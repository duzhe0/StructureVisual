qt是个工具箱
什么是对象树管理：我认为提供了对象的创建、复制、删除的管理。避免了空间泄漏
事件处理指的是什么
qt的元对象系统是什么
什么特殊处理
反射功能是什么
Q_OBJECT宏在声明本类调用了工具箱中工具
QWidget是用户界面，封装了显示，接收用户输入、处理事件的接口，供业务逻辑调用
QMainWindow继承QWidget，自带一套标准布局
类要用signal必须继承QObject，在类中声明具体如下：
class AlgorithmController:public QObject
{
    Q_OBJECT
signals:
    void algorithmStarted();
    void algorithmPaused();
    void stepCompleted(const QString &description);
}
algorithmStarted()方法不需要具体实现
如何确定特定事件与特定signal的关系
类要使用槽函数必须继承QObject,类中声明如下：
private slot:
    void onAlgorithmStarted();
槽函数是普通c++函数,有输入有返回但是通常信号槽连接中不使用
什么是“信号槽连接中不使用”，“不使用什么”，“为什么不使用”
可以接收信号传递的参数
connect(m_speedSlider,&QSlider::valueChanged,this,&AlgorithmController::setAnimationSpeed);
connect(谁变化，发出的信号种类，要启动哪里的槽函数，槽函数的地址)
槽函数规定了哪些函数可以接收信号，槽函数的签名是要求，只有签名大于等于槽函数的信号才能与之连接
槽函数的private，protect,public影响谁能设置槽函数与信号的连接-封装性
连接(connect)可以在接收信号的组件init，专门设置函数在需要时调用
信号是给别人看的，不能是private，信号有签名，不需要实现

| 特性 | 直接函数调用（专事专办） | 信号槽（卖报纸） |
|------|------------------------|-----------------|
| 执行方式 | 同步，立即执行 | 异步，发出通知 |
| 等待方式 | 在线等，阻塞等待 | 等通知，不阻塞 |
| 返回值 | 有返回值 | 无返回值 |
| 关系 | 一对一 | 一对多 |
| 使用场景 | 需要立即结果的操作 | 状态通知、事件响应 |
| 比喻 | 专事专办-在线等 | 卖报纸-等通知-一对多 |
┌─────────────────────────────────────────────────────────────┐
│ 方向1：用户输入流（View → Controller → Model）              │
└─────────────────────────────────────────────────────────────┘
View → Controller:  ✅ 使用信号槽（emit + connect）
Controller → Model: ❌ 直接函数调用（普通C++调用）
用户点击按钮
    ↓
View层：QPushButton 发出 clicked 信号
    ↓
Controller层：onStartButtonClicked() 槽函数接收
    ↓
Controller层：startAlgorithm() 调用
    ↓
Model层：executeAlgorithm() 执行算法


┌─────────────────────────────────────────────────────────────┐
│ 方向2：状态通知流（Model → Controller → View）              │
└─────────────────────────────────────────────────────────────┘
Model → Controller: ✅ 使用信号槽（emit + connect）
Controller → View:  ✅ 使用信号槽（emit + connect）
Model层：算法执行完成
    ↓
Model层：emit algorithmCompleted() 发出信号
    ↓
Controller层：onAlgorithmCompleted() 槽函数接收
    ↓
Controller层：emit algorithmCompleted() 转发信号
    ↓
View层：MainWindow::onAlgorithmCompleted() 接收
    ↓
View层：更新UI（状态栏、进度条等）