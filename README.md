## 【*】 程序简介 
- 工程名称：STM32F407 UC-OS3学习记录
- 实验平台: STM32F407最小系统板
- MDK版本：5.16
- ST固件库版本：1.8.0
- 参考工程：秉火STM32 F407 开发板 UC-OS3的工程
### 【-】 时钟
A. 晶振：
- 外部高速晶振：8MHz
- RTC晶振：32.768KHz

B. 各总线运行时钟：
- 系统时钟 = SYCCLK = AHB1 = 168MHz
- APB2 = 84MHz 
- APB1 = 42MHz

C. 浮点运算单元：
- 不使用

### 【 ！】功能简介：
在STM32F407上逐步使用UC-OS3的各种feature

学习目的：掌握UC-OS3。  
大纲：
1. 串口的数据发送，LED的点亮，任务的创建，挂起与恢复
2. 按键切换LED点亮任务的挂起和恢复
3. I2C通信，点亮0.96寸I2C IIC通信 128*64 OLED液晶屏模块（驱动是SSD1315）,显示字符串和汉字
4. 使用内部空闲的FLASH持久化数据，并在断电重启时再次获取数据

### 【 ！】实验
#### 实验一
##### 目的：
1. 查看CPU内部的时钟频率是否设置正确，并串口发送到电脑
2. 创建LED2,LED3的任务
3. 学习任务的挂起与恢复(OSTaskSuspend,OSTaskResume)
##### 操作：
1. 电脑端使用串口调试助手，选择电脑与STM32相连的COM口，设置为115200-N-8-1，
2. 复位开发板，即可接收STM32串口发送给电脑的数据。
##### 现象：
1. LED2, LED3每秒翻转一次
2. LED3 每5秒挂起自身一次
3. LED2 每10秒恢复LED3（LED3重新点亮并继续每s翻转一次）
4. 重复上述现象

#### 实验二
##### 目的：
1. 学习按键输入的检测
2. 确认任务多次挂起后需要相同数量的恢复
##### 操作：
1. 创建事件标志组 `OSFlagCreate()`
2. 按键按下给事件标志组对应位置一，当两个按键都按下过后`OSFlagPend`会返回数据
3. 电脑端使用串口调试助手，选择电脑与STM32相连的COM口，设置为115200-N-8-1，
4. 复位开发板，即可接收STM32串口发送给电脑的数据。
##### 现象：
1. LED2, LED3每秒翻转一次
2. LED3 每5秒挂起自身一次
3. LED2 每10秒恢复LED3（LED3重新点亮并继续每s翻转一次）
4. 重复上述现象
5. K0按下挂起/恢复LED2的任务
6. K1按下挂起LED3的任务, 只能循环等待相同次数的LED2恢复

#### 实验三
##### 目的：
1. 点亮驱动为SSD1315的OLED，学习I2C通信
##### 操作：
1. 选择I2C2作为连接端口（PB10,PB11）
2. 启动GPIOB和I2C2时钟，配置Pin和复用Pin, 配置I2C
3. 初始化OLED, OLED打开显示，清屏`OLED_Display_On(); OLED_Clear();`
4. 显示字符串和汉字
##### 现象：
OLED显示字符串hello world!!和汉字‘尖’

#### 实验四
##### 目的：
1. 使用STM32内部空闲的FLASH，持久化保存数据，并在重新上电运行时读取数据
##### 操作：
1. 选择一块空闲的内存卡作为存储FLASH，可以通过Listing/xxx.map，如下所示，计算得到空闲位置：
```json
  Memory Map of the image

  Image Entry point : 0x08000189

  Load Region LR_IROM1 (Base: 0x08000000, Size: 0x000080b4, Max: 0x00080000, ABSOLUTE, COMPRESSED[0x00007f8c])

    Execution Region ER_IROM1 (Exec base: 0x08000000, Load base: 0x08000000, Size: 0x00007f28, Max: 0x00080000, ABSOLUTE)

    Exec Addr    Load Addr    Size         Type   Attr      Idx    E Section Name        Object
```
1. 按下K0,向空闲FLASH内写入数据(注意数据要对齐,写入读取时是按照32位操作的)
2. 断电，重新上电后，按下K1，从空闲内存块读取数据，并显示到OLED上
##### 现象：
1. 数据写入到空闲FLASH
2. 断电重启后可以再次读取数据并显示
