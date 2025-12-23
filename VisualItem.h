#ifndef VISUALITEM_H
#define VISUALITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QPainterPathStroker>

// 可视化元素状态枚举
enum class VisualState {
    Normal,      // 正常状态
    Highlighted, // 高亮状态
    Visited,     // 已访问状态
    Current,     // 当前状态
    Selected,    // 选中状态
    Processing   // 处理中状态
};

// 可视化元素基类
class VisualItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit VisualItem(QGraphicsItem *parent = nullptr);
    virtual ~VisualItem() = default;

    // 状态管理
    void setVisualState(VisualState state);
    VisualState getVisualState() const { return m_state; }
    
    // 动画控制
    void startHighlightAnimation();
    void startPulseAnimation();
    void stopAnimations();
    
    // 颜色管理
    void setBaseColor(const QColor &color);
    QColor getBaseColor() const { return m_baseColor; }
    
    // 文本管理
    void setText(const QString &text);
    QString getText() const { return m_text; }
    
    // 位置管理
    void setPosition(const QPointF &position);
    QPointF getPosition() const { return pos(); }

protected:
    // QGraphicsItem 接口
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    
    // 状态颜色映射
    QColor getStateColor() const;
    
    // 动画效果
    void updateVisualEffects();

private:
    VisualState m_state;
    QColor m_baseColor;
    QString m_text;
    QFont m_font;
    
    // 动画相关
    QPropertyAnimation *m_highlightAnimation;
    QPropertyAnimation *m_pulseAnimation;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    // 动画属性 - 使用Qt内置属性
    qreal m_highlightOpacity;
    qreal m_pulseScale;
};

// 顶点可视化项
class VertexItem : public VisualItem
{
    Q_OBJECT

public:
    explicit VertexItem(const QString &label, const QPointF &position, QGraphicsItem *parent = nullptr);
    ~VertexItem() = default;

    // 顶点特有属性
    void setRadius(qreal radius);
    qreal getRadius() const { return m_radius; }
    
    void setLabel(const QString &label);
    QString getLabel() const { return m_label; }
    
    // 顶点动画
    void startInsertAnimation();
    void startDeleteAnimation();
    void startVisitAnimation();

signals:
    // 当顶点位置改变时发出信号
    void positionChanged(const QPointF &newPosition);

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    // 重写itemChange以检测位置变化
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QString m_label;
    qreal m_radius;
    QColor m_borderColor;
    int m_borderWidth;
};

// 边可视化项
class EdgeItem : public VisualItem
{
    Q_OBJECT

public:
    explicit EdgeItem(VertexItem *from, VertexItem *to, int weight = 0, bool isDirected = true, QGraphicsItem *parent = nullptr);
    ~EdgeItem() = default;

    // 边特有属性
    void setWeight(int weight);
    int getWeight() const { return m_weight; }
    
    void setFromVertex(VertexItem *vertex);
    void setToVertex(VertexItem *vertex);
    VertexItem* getFromVertex() const { return m_fromVertex; }
    VertexItem* getToVertex() const { return m_toVertex; }
    
    // 边动画
    void startTraverseAnimation();
    void startSelectAnimation();
    
    // 更新位置（当顶点移动时）
    void updatePosition();

protected:
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    // 计算边的几何信息（在局部坐标系中）
    QPointF getStartPoint() const;
    QPointF getEndPoint() const;
    QPointF getWeightLabelPosition() const;
    
    // 连接顶点位置变化信号
    void connectVertexSignals();
    void disconnectVertexSignals();

private:
    VertexItem *m_fromVertex;
    VertexItem *m_toVertex;
    int m_weight;
    QColor m_lineColor;
    int m_lineWidth;
    bool m_isDirected;
    
    // 箭头绘制
    void drawArrow(QPainter *painter, const QPointF &start, const QPointF &end);
    QPolygonF createArrowHead(const QPointF &start, const QPointF &end, qreal arrowSize = 10.0);
    
    // 边宽度（用于shape()计算）
    qreal m_edgeWidth;
    
private slots:
    // 当顶点位置变化时自动更新边
    void onVertexPositionChanged();
};

// 排序柱状图项
class BarItem : public VisualItem
{
public:
    explicit BarItem(int value, int index, QGraphicsItem *parent = nullptr);
    ~BarItem() = default;

    // 柱状图特有属性
    void setValue(int value);
    int getValue() const { return m_value; }
    
    void setIndex(int index);
    int getIndex() const { return m_index; }
    
    void setBarWidth(qreal width);
    qreal getBarWidth() const { return m_barWidth; }
    
    // 柱状图动画
    void startCompareAnimation();
    void startSwapAnimation(const QPointF &targetPosition);
    void startSelectAnimation();

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int m_value;
    int m_index;
    qreal m_barWidth;
    qreal m_maxHeight;
    QColor m_barColor;
    QColor m_textColor;
    
    // 计算柱状图高度
    qreal calculateBarHeight() const;
};

#endif // VISUALITEM_H
