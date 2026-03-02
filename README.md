**项目名称：**外墙喷涂设备数字孪生系统

**Unity版本：**2022.3.50f1

**渲染管线：**URP

**文件结构：**

- <DT_Test> 项目文件 ：在Unity Hub中Add Project打开文件夹
- <DT_exe> 可执行文件
  - 双击DT_Test.exe文件运行



**操作指南：**

- **运行：**点击UI上的“开始模拟”按钮启动
- **重置：**点击停止模拟，再次点击开始会自动重置墙面和机器人状态。
- **视角控制**：
  - **漫游模式**：按住 **鼠标右键** + **WASD** 移动。
  - **快捷切换**：点击 UI 上的 **“视图：全景/墙面”** 按钮。
- **数据查看**：点击墙面任意位置，UI 会显示该点的局部坐标和厚度数据。



**项目常见问题-粉色材质修复：**

- 如果打开场景发现物体变成粉色
  - 点击顶部菜单 **Window** -> **Rendering** -> **Render Pipeline Converter**。
  - 在弹窗中选择 **Material Upgrade**。
  - 勾选所有材质，点击 **Initialize** 和 **Convert**。
  - 材质即可恢复正常。



**项目内文件结构：**

- Materials：存放shader和材质
- Models：导入的.fbx模型
- Prefabs：预制体
- Scenes：场景，目前主场景为OutwallTest，其余几个用于之前的测试
- Scripts：**核心代码分类和逻辑**
  - Models：底层模型，原始数据结构
  - ViewModel：中间层，处理数据，发布给上层view层
  - View：将接收到的信息转换为unity的可视化
    - RobotMovementDriver：控制移动和管理喷嘴
    - RobotSprayNozzle：单个喷嘴的喷涂行为
  - UI：UI相关逻辑
    - ThicknessReader：获取鼠标点击点的膜厚值
    - ThicknessClickUI：根据当前摄像机和鼠标 点击位置，显示出膜厚值
    - DashboardMonitor：连接并显示实时参数值在面板上
    - CameraController：连接UI，控制摄像机操作，可按钮操作切换视角和是否隐藏机架
    - SimulatorController：连接UI，控制模拟的开始和重置
  - Utilities：
    - RobotSimulator：目前做机器人数据模拟的脚本
    - FreeFlyCamera：主视角下按下右键可移动逻辑，可与UI-CameraController集成在一起
    - GradientGen：自定义热力图生成工具
    - PaintableObject：可喷涂的物体逻辑，挂载在喷涂的物体上【例：Prefabs/Paintable Outwall】
    - SprayController：之前做鼠标喷涂测试时的脚本（可略）
    - SimpleOscillator：振荡运动模拟的脚本（可略）



**目前问题：**

- 掉帧会影响喷涂效果，如卡顿导致涂料堆积