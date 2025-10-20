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
    scaleAnim->setEasingCurve(QEasingCurve::OutBack);
    
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
    scaleAnim->setEasingCurve(QEasingCurve::InBack);
    
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

// ==================== EdgeItem 实现 ====================

EdgeItem::EdgeItem(VertexItem *from, VertexItem *to, int weight, bool isDirected, QGraphicsItem *parent)
    : VisualItem(parent)
    , m_fromVertex(from)
    , m_toVertex(to)
    , m_weight(weight)
    , m_lineColor(QColor(0, 0, 0)) // black
    , m_lineWidth(2)
    , m_isDirected(isDirected)
{
    setBaseColor(QColor(0, 0, 0)); // black
    updatePosition();
}

QRectF EdgeItem::boundingRect() const
{
    QPointF start = getStartPoint();
    QPointF end = getEndPoint();
    
    qreal minX = qMin(start.x(), end.x()) - 20;
    qreal maxX = qMax(start.x(), end.x()) + 20;
    qreal minY = qMin(start.y(), end.y()) - 20;
    qreal maxY = qMax(start.y(), end.y()) + 20;
    
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    if (!m_fromVertex || !m_toVertex) return;
    
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
        m_fromVertex = vertex;
        updatePosition();
    }
}

void EdgeItem::setToVertex(VertexItem *vertex)
{
    if (m_toVertex != vertex) {
        m_toVertex = vertex;
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
        // 更新边的位置，使其始终连接两个顶点
        QPointF start = getStartPoint();
        QPointF end = getEndPoint();
        setPos((start + end) / 2);
    }
}

QPointF EdgeItem::getStartPoint() const
{
    if (!m_fromVertex) return QPointF();
    
    QPointF vertexPos = m_fromVertex->pos();
    QPointF edgePos = pos();
    QPointF direction = edgePos - vertexPos;
    qreal distance = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (distance > 0) {
        direction /= distance;
        return vertexPos + direction * m_fromVertex->getRadius();
    }
    return vertexPos;
}

QPointF EdgeItem::getEndPoint() const
{
    if (!m_toVertex) return QPointF();
    
    QPointF vertexPos = m_toVertex->pos();
    QPointF edgePos = pos();
    QPointF direction = vertexPos - edgePos;
    qreal distance = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (distance > 0) {
        direction /= distance;
        return vertexPos - direction * m_toVertex->getRadius();
    }
    return vertexPos;
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
    return QRectF(-m_barWidth/2, -calculateBarHeight(), m_barWidth, calculateBarHeight());
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
