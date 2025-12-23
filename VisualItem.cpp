#include "VisualItem.h"
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QtMath>

// ==================== VisualItem 基类实现 ====================

VisualItem::VisualItem(QGraphicsItem *parent)
    : QObject(), QGraphicsItem(parent)
    , m_state(VisualState::Normal)
    , m_baseColor(QColor(0, 0, 255)) // blue
    , m_font("Arial", 12)
    , m_highlightAnimation(nullptr)
    , m_pulseAnimation(nullptr)
    , m_shadowEffect(nullptr)
    , m_highlightOpacity(1.0)
    , m_pulseScale(1.0)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    // 初始化阴影效果
    m_shadowEffect = new QGraphicsDropShadowEffect();
    m_shadowEffect->setBlurRadius(10);
    m_shadowEffect->setOffset(2, 2);
    m_shadowEffect->setColor(QColor(0, 0, 0, 100));
    setGraphicsEffect(m_shadowEffect);
    
    // 初始化动画 - 使用Qt 6语法
    m_highlightAnimation = new QPropertyAnimation();
    m_highlightAnimation->setTargetObject(this);
    m_highlightAnimation->setPropertyName("opacity");
    m_highlightAnimation->setDuration(1000);
    m_highlightAnimation->setLoopCount(-1);
    m_highlightAnimation->setKeyValueAt(0, 1.0);
    m_highlightAnimation->setKeyValueAt(0.5, 0.3);
    m_highlightAnimation->setKeyValueAt(1, 1.0);
    
    m_pulseAnimation = new QPropertyAnimation();
    m_pulseAnimation->setTargetObject(this);
    m_pulseAnimation->setPropertyName("scale");
    m_pulseAnimation->setDuration(800);
    m_pulseAnimation->setLoopCount(-1);
    m_pulseAnimation->setKeyValueAt(0, 1.0);
    m_pulseAnimation->setKeyValueAt(0.5, 1.2);
    m_pulseAnimation->setKeyValueAt(1, 1.0);
}

QRectF VisualItem::boundingRect() const
{
    return QRectF(-50, -50, 100, 100);
}

void VisualItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // 基础绘制逻辑，子类会重写
    painter->setPen(QPen(getStateColor(), 2));
    painter->setBrush(QBrush(getStateColor().lighter(150)));
    painter->drawEllipse(boundingRect());
    
    // 绘制文本
    if (!m_text.isEmpty()) {
        painter->setPen(QColor(0, 0, 0)); // black
        painter->setFont(m_font);
        painter->drawText(boundingRect(), Qt::AlignCenter, m_text);
    }
}

void VisualItem::setVisualState(VisualState state)
{
    if (m_state != state) {
        m_state = state;
        updateVisualEffects();
        update();
    }
}

void VisualItem::setBaseColor(const QColor &color)
{
    if (m_baseColor != color) {
        m_baseColor = color;
        update();
    }
}

void VisualItem::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        update();
    }
}

void VisualItem::setPosition(const QPointF &position)
{
    setPos(position);
}

void VisualItem::startHighlightAnimation()
{
    stopAnimations();
    m_highlightAnimation->start();
}

void VisualItem::startPulseAnimation()
{
    stopAnimations();
    m_pulseAnimation->start();
}

void VisualItem::stopAnimations()
{
    m_highlightAnimation->stop();
    m_pulseAnimation->stop();
    setOpacity(1.0);
    setScale(1.0);
    /*调用 update() 后，Qt 会标记这个图形项需要重绘
    Qt 会在合适的时机调用 paint() 函数重新绘制，
    界面会显示更新后的内容 */
    update();
}

QColor VisualItem::getStateColor() const
{
    switch (m_state) {
        case VisualState::Normal:
            return m_baseColor;
        case VisualState::Highlighted:
            return m_baseColor.lighter(120);
        case VisualState::Visited:
            return QColor(100, 200, 100); // 绿色
        case VisualState::Current:
            return QColor(255, 165, 0);   // 橙色
        case VisualState::Selected:
            return QColor(255, 0, 0);     // 红色
        case VisualState::Processing:
            return QColor(255, 255, 0);   // 黄色
        default:
            return m_baseColor;
    }
}

void VisualItem::updateVisualEffects()
{
    switch (m_state) {
        case VisualState::Highlighted:
            m_shadowEffect->setColor(QColor(255, 255, 0, 150));
            m_shadowEffect->setBlurRadius(15);
            break;
        case VisualState::Current:
            m_shadowEffect->setColor(QColor(255, 165, 0, 150));
            m_shadowEffect->setBlurRadius(20);
            break;
        case VisualState::Selected:
            m_shadowEffect->setColor(QColor(255, 0, 0, 150));
            m_shadowEffect->setBlurRadius(25);
            break;
        default:
            m_shadowEffect->setColor(QColor(0, 0, 0, 100));
            m_shadowEffect->setBlurRadius(10);
            break;
    }
}

QVariant VisualItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // 基类默认实现，直接返回value
    // 子类可以重写此方法以处理特定的变化
    return QGraphicsItem::itemChange(change, value);
}

// ==================== VertexItem 实现 ====================

VertexItem::VertexItem(const QString &label, const QPointF &position, QGraphicsItem *parent)
    : VisualItem(parent)
    , m_label(label)
    , m_radius(25.0)
    , m_borderColor(QColor(0, 0, 0)) // black
    , m_borderWidth(2)
{
    setText(label);
    setPosition(position);
    setBaseColor(QColor(100, 150, 255));
}

QRectF VertexItem::boundingRect() const
{
    qreal margin = m_borderWidth + 5;
    //左上角坐标 宽高
    return QRectF(-m_radius - margin, -m_radius - margin, 
                  (m_radius + margin) * 2, (m_radius + margin) * 2);
}

void VertexItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // 绘制顶点圆圈
    QColor fillColor = getStateColor();
    painter->setPen(QPen(m_borderColor, m_borderWidth));
    painter->setBrush(QBrush(fillColor));
    painter->drawEllipse(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
    
    // 绘制标签
    if (!m_label.isEmpty()) {
        painter->setPen(QColor(0, 0, 0)); // black
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(QRectF(-m_radius, -m_radius, m_radius * 2, m_radius * 2), 
                         Qt::AlignCenter, m_label);
    }
}

void VertexItem::setRadius(qreal radius)
{
    if (m_radius != radius) {
        m_radius = radius;
        update();
    }
}

void VertexItem::setLabel(const QString &label)
{
    if (m_label != label) {
        m_label = label;
        setText(label);
    }
}

void VertexItem::startInsertAnimation()
{
    setScale(0.0);
    setOpacity(0.0);
    
    QPropertyAnimation *scaleAnim = new QPropertyAnimation();
    scaleAnim->setTargetObject(this);
    scaleAnim->setPropertyName("scale");
    scaleAnim->setDuration(500);
    scaleAnim->setStartValue(0.0);
    scaleAnim->setEndValue(1.0);
    scaleAnim->setEasingCurve(QEasingCurve::OutBack);//OutBack 后退缓动曲线
    
    QPropertyAnimation *opacityAnim = new QPropertyAnimation();
    opacityAnim->setTargetObject(this);
    opacityAnim->setPropertyName("opacity");
    opacityAnim->setDuration(300);
    opacityAnim->setStartValue(0.0);
    opacityAnim->setEndValue(1.0);
    
    scaleAnim->start();
    opacityAnim->start();
}

void VertexItem::startDeleteAnimation()
{
    QPropertyAnimation *scaleAnim = new QPropertyAnimation();
    scaleAnim->setTargetObject(this);
    scaleAnim->setPropertyName("scale");
    scaleAnim->setDuration(300);
    scaleAnim->setStartValue(1.0);
    scaleAnim->setEndValue(0.0);
    scaleAnim->setEasingCurve(QEasingCurve::InBack);//InBack 前进缓动曲线
    
    QPropertyAnimation *opacityAnim = new QPropertyAnimation();
    opacityAnim->setTargetObject(this);
    opacityAnim->setPropertyName("opacity");
    opacityAnim->setDuration(300);
    opacityAnim->setStartValue(1.0);
    opacityAnim->setEndValue(0.0);
    
    connect(opacityAnim, &QPropertyAnimation::finished, this, [this]() {
        scene()->removeItem(this);
        delete this;
    });
    
    scaleAnim->start();
    opacityAnim->start();
}

void VertexItem::startVisitAnimation()
{
    startPulseAnimation();
    setVisualState(VisualState::Visited);
}

QVariant VertexItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // 当位置改变时，发出positionChanged信号
    if (change == ItemPositionHasChanged) {
        QPointF newPos = value.toPointF();
        emit positionChanged(newPos);
    }
    
    // 调用基类实现
    return VisualItem::itemChange(change, value);
}

// ==================== EdgeItem 实现 ====================

EdgeItem::EdgeItem(VertexItem *from, VertexItem *to, int weight, bool isDirected, QGraphicsItem *parent)
    : VisualItem(parent)
    , m_fromVertex(from)
    , m_toVertex(to)
    , m_weight(weight)
    , m_lineColor(QColor(0, 0, 0)) // black
    , m_lineWidth(2)
    , m_isDirected(isDirected)
    , m_edgeWidth(m_lineWidth + 8) // 边宽度：线宽 + 边距（用于碰撞检测）
{
    setBaseColor(QColor(0, 0, 0)); // black
    
    // 注意：重写了shape()方法后，Qt会自动使用它进行碰撞检测
    // 不需要显式设置模式
    
    // 禁止直接移动边，只能由顶点位置变化引起
    setFlag(QGraphicsItem::ItemIsMovable, false);
    
    // 初始化位置为两个顶点的中点
    if (m_fromVertex && m_toVertex) {
        QPointF fromPos = m_fromVertex->pos();
        QPointF toPos = m_toVertex->pos();
        QPointF centerPos = (fromPos + toPos) / 2;
        setPos(centerPos);
    }
    
    // 连接顶点位置变化信号
    connectVertexSignals();
}

QRectF EdgeItem::boundingRect() const
{
    // 基于shape()计算边界矩形，不使用QRectF直接构造
    // 这样确保边界矩形与边的方向一致
    QPainterPath path = shape();
    return path.boundingRect();
}

QPainterPath EdgeItem::shape() const
{
    QPainterPath path;
    
    if (!m_fromVertex || !m_toVertex) {
        return path;
    }
    
    // 获取起点和终点（在局部坐标系中）
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    
    // 创建边的路径（从起点到终点）
    path.moveTo(start);
    path.lineTo(end);
    
    // 计算边的方向向量（用于创建与边平行的矩形）
    QPointF direction = end - start;
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (length == 0) {
        // 如果起点和终点相同，返回一个小的圆形区域
        path.addEllipse(start, m_edgeWidth / 2, m_edgeWidth / 2);
        return path;
    }
    
    // 归一化方向向量
    direction /= length;
    
    // 计算垂直于边的向量（用于创建矩形宽度）
    QPointF perpendicular(-direction.y(), direction.x());
    
    // 计算矩形半宽（用于碰撞检测和边界计算）
    qreal halfWidth = m_edgeWidth / 2;
    
    // 创建与边平行的矩形路径
    // 矩形的四个顶点：
    QPointF p1 = start + perpendicular * halfWidth;  // 起点上方
    QPointF p2 = start - perpendicular * halfWidth;  // 起点下方
    QPointF p3 = end - perpendicular * halfWidth;    // 终点下方
    QPointF p4 = end + perpendicular * halfWidth;    // 终点上方
    
    // 构建矩形路径
    path = QPainterPath();
    path.moveTo(p1);
    path.lineTo(p4);
    path.lineTo(p3);
    path.lineTo(p2);
    path.closeSubpath();
    
    // 为箭头和权重标签添加额外区域（如果有）
    if (m_isDirected || m_weight > 0) {
        qreal extraMargin = 15.0; // 箭头和标签的额外空间
        
        // 在终点处扩展区域以包含箭头
        if (m_isDirected) {
            QPointF arrowBase = end - direction * 10; // 箭头基础位置
            QPainterPath arrowPath;
            arrowPath.addEllipse(arrowBase, extraMargin, extraMargin);
            path = path.united(arrowPath);
        }
        
        // 在边的中点扩展区域以包含权重标签
        if (m_weight > 0) {
            QPointF labelCenter = (start + end) / 2;
            QPainterPath labelPath;
            labelPath.addEllipse(labelCenter, extraMargin, extraMargin);
            path = path.united(labelPath);
        }
    }
    
    return path;
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    if (!m_fromVertex || !m_toVertex) return;
    
    // 获取起点和终点（在局部坐标系中）
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    
    // 绘制边线
    QColor lineColor = getStateColor();
    painter->setPen(QPen(lineColor, m_lineWidth));
    painter->drawLine(start, end);
    
    // 绘制箭头（有向图）
    if (m_isDirected) {
        drawArrow(painter, start, end);
    }
    
    // 绘制权重标签
    if (m_weight > 0) {
        QPointF labelPos = getWeightLabelPosition();
        painter->setPen(QColor(0, 0, 0)); // black
        painter->setFont(QFont("Arial", 8));
        painter->setBrush(QColor(255, 255, 255)); // white
        painter->drawEllipse(labelPos.x() - 8, labelPos.y() - 8, 16, 16);
        painter->drawText(QRectF(labelPos.x() - 8, labelPos.y() - 8, 16, 16), 
                         Qt::AlignCenter, QString::number(m_weight));
    }
}

void EdgeItem::setWeight(int weight)
{
    if (m_weight != weight) {
        m_weight = weight;
        update();
    }
}

void EdgeItem::setFromVertex(VertexItem *vertex)
{
    if (m_fromVertex != vertex) {
        // 断开旧顶点的信号连接
        disconnectVertexSignals();
        
        m_fromVertex = vertex;
        
        // 重新连接信号
        connectVertexSignals();
        
        // 更新位置
        updatePosition();
    }
}

void EdgeItem::setToVertex(VertexItem *vertex)
{
    if (m_toVertex != vertex) {
        // 断开旧顶点的信号连接
        disconnectVertexSignals();
        
        m_toVertex = vertex;
        
        // 重新连接信号
        connectVertexSignals();
        
        // 更新位置
        updatePosition();
    }
}

void EdgeItem::startTraverseAnimation()
{
    setVisualState(VisualState::Current);
    startHighlightAnimation();
}

void EdgeItem::startSelectAnimation()
{
    setVisualState(VisualState::Selected);
    startPulseAnimation();
}

void EdgeItem::updatePosition()
{
    if (m_fromVertex && m_toVertex) {
        // 直接使用两个顶点的位置计算边的中心位置
        // 不依赖getStartPoint()和getEndPoint()，避免循环调用
        QPointF fromPos = m_fromVertex->pos();
        QPointF toPos = m_toVertex->pos();
        QPointF centerPos = (fromPos + toPos) / 2;
        
        // 只有当位置真正改变时才更新，避免无限循环
        if (pos() != centerPos) {
            prepareGeometryChange(); // 通知Qt几何形状即将改变
            setPos(centerPos);
            update(); // 触发重绘
        }
    }
}

QPointF EdgeItem::getStartPoint() const
{
    // 直接返回起点顶点的中心位置（在局部坐标系中）
    // 不依赖半径，不依赖边的pos()，避免循环调用
    if (!m_fromVertex || !m_toVertex) return QPointF();
    
    // 获取顶点在场景中的位置
    QPointF fromPos = m_fromVertex->pos();
    QPointF toPos = m_toVertex->pos();
    
    // 计算边的中心位置（边的pos()）
    QPointF edgeCenter = (fromPos + toPos) / 2;
    
    // 将起点顶点位置转换到边的局部坐标系
    return fromPos - edgeCenter;
}

QPointF EdgeItem::getEndPoint() const
{
    // 直接返回终点顶点的中心位置（在局部坐标系中）
    // 不依赖半径，不依赖边的pos()，避免循环调用
    if (!m_fromVertex || !m_toVertex) return QPointF();
    
    // 获取顶点在场景中的位置
    QPointF fromPos = m_fromVertex->pos();
    QPointF toPos = m_toVertex->pos();
    
    // 计算边的中心位置（边的pos()）
    QPointF edgeCenter = (fromPos + toPos) / 2;
    
    // 将终点顶点位置转换到边的局部坐标系
    return toPos - edgeCenter;
}

QPointF EdgeItem::getWeightLabelPosition() const
{
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    return (start + end) / 2;
}

void EdgeItem::drawArrow(QPainter *painter, const QPointF &start, const QPointF &end)
{
    QPolygonF arrowHead = createArrowHead(start, end);
    painter->setBrush(QBrush(getStateColor()));
    painter->drawPolygon(arrowHead);
}

QPolygonF EdgeItem::createArrowHead(const QPointF &start, const QPointF &end, qreal arrowSize)
{
    QPointF direction = end - start;
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (length == 0) return QPolygonF();
    
    direction /= length;
    
    QPointF arrowP1 = end - direction * arrowSize + QPointF(-direction.y(), direction.x()) * arrowSize * 0.5;
    QPointF arrowP2 = end - direction * arrowSize + QPointF(direction.y(), -direction.x()) * arrowSize * 0.5;
    
    QPolygonF arrowHead;
    arrowHead << end << arrowP1 << arrowP2;
    return arrowHead;
}

void EdgeItem::connectVertexSignals()
{
    // 断开所有现有连接，避免重复连接
    disconnectVertexSignals();
    
    // 连接起点顶点的位置变化信号
    if (m_fromVertex) {
        connect(m_fromVertex, &VertexItem::positionChanged, this, &EdgeItem::onVertexPositionChanged);
    }
    
    // 连接终点顶点的位置变化信号
    if (m_toVertex) {
        connect(m_toVertex, &VertexItem::positionChanged, this, &EdgeItem::onVertexPositionChanged);
    }
}

void EdgeItem::disconnectVertexSignals()
{
    // 断开起点顶点的信号连接
    if (m_fromVertex) {
        disconnect(m_fromVertex, &VertexItem::positionChanged, this, &EdgeItem::onVertexPositionChanged);
    }
    
    // 断开终点顶点的信号连接
    if (m_toVertex) {
        disconnect(m_toVertex, &VertexItem::positionChanged, this, &EdgeItem::onVertexPositionChanged);
    }
}

void EdgeItem::onVertexPositionChanged()
{
    // 当顶点位置变化时，自动更新边的位置和方向
    // 这个方法由信号槽机制自动调用，不能直接移动边
    updatePosition();
}

// ==================== BarItem 实现 ====================

BarItem::BarItem(int value, int index, QGraphicsItem *parent)
    : VisualItem(parent)
    , m_value(value)
    , m_index(index)
    , m_barWidth(30.0)
    , m_maxHeight(200.0)
    , m_barColor(QColor(100, 150, 255))
    , m_textColor(QColor(0, 0, 0)) // black
{
    setBaseColor(m_barColor);
    setText(QString::number(value));
}

QRectF BarItem::boundingRect() const
{
    qreal barHeight = calculateBarHeight();
    // 预留顶部空间用于显示数值标签，避免被裁剪
    return QRectF(-m_barWidth/2, -barHeight - 24, m_barWidth, barHeight + 24);
}

void BarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    qreal barHeight = calculateBarHeight();
    QColor fillColor = getStateColor();
    
    // 绘制柱状图
    painter->setPen(QPen(QColor(0, 0, 0), 1)); // black
    painter->setBrush(QBrush(fillColor));
    painter->drawRect(-m_barWidth/2, -barHeight, m_barWidth, barHeight);
    
    // 绘制数值标签
    painter->setPen(m_textColor);
    painter->setFont(QFont("Arial", 8));
    painter->drawText(QRectF(-m_barWidth/2, -barHeight - 20, m_barWidth, 20), 
                     Qt::AlignCenter, QString::number(m_value));
}

void BarItem::setValue(int value)
{
    if (m_value != value) {
        m_value = value;
        setText(QString::number(value));
        update();
    }
}

void BarItem::setIndex(int index)
{
    if (m_index != index) {
        m_index = index;
        update();
    }
}

void BarItem::setBarWidth(qreal width)
{
    if (m_barWidth != width) {
        m_barWidth = width;
        update();
    }
}

void BarItem::startCompareAnimation()
{
    setVisualState(VisualState::Current);
    startPulseAnimation();
}

void BarItem::startSwapAnimation(const QPointF &targetPosition)
{
    QPropertyAnimation *moveAnim = new QPropertyAnimation();
    moveAnim->setTargetObject(this);
    moveAnim->setPropertyName("pos");
    moveAnim->setDuration(500);
    moveAnim->setStartValue(pos());
    moveAnim->setEndValue(targetPosition);
    moveAnim->setEasingCurve(QEasingCurve::InOutQuad);
    moveAnim->start();
    
    startHighlightAnimation();
}

void BarItem::startSelectAnimation()
{
    setVisualState(VisualState::Selected);
    startHighlightAnimation();
}

qreal BarItem::calculateBarHeight() const
{
    // 假设最大值为100，可以根据实际情况调整
    const int maxValue = 100;
    return (static_cast<qreal>(m_value) / maxValue) * m_maxHeight;
}
