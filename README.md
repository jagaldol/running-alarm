# 달리는 알람(embedded project)

## 2022년 임베디드 시스템 설계 및 실험 Term Project
- 특정 시간이 되면 달리는 알람 시계를 개발한다.
- 달리는 기기는 벽을 최대한 피하고, 사람 손에 잘 닿지 않도록 한다.
- 기기의 목적은 사용자가 침대로부터 일어나 걸음을 걸어 일정을 시작하도록 하는 것이다.

> 특정한 시간에 달리는 알람 시계를 개발하고, bluetooth로 알람을 울릴 시간을 제어한다. 달리는 알람 시계는 벽에 부딪히지 않도록 한다.

![product_image](/image/product_image.jpg)

 ---

 ## 환경
- IAR Embedded Workbench IDE - Arm 9.30.1
- 개발보드 : STM32F107VCT6

## 사용 센서 및 모듈
 - DS3231 RTC 고정밀 리얼타임 클럭 모듈 (SZH-EK047)
 - 초음파 거리센서 모듈 (HC-SR04)
 - 인체감지센서모듈 HC-SR501 (SZH-EK052)
 - Bluetooth 모듈 (FB755AC)
 - 알람 경보용 피에조 부저
 - TFT-LCD

## 작동
### 알람 설정
- 블루투스 통신을 통해 기기로 알람을 울리라는 신호를 발생시킨다.

### 기기의 작동
- 신호를 받은 기기는 부저를 울리고, 모터를 작동해 달리기 시작한다.
- 기기가 벽에 닿으려 할 시, 초음파 센서를 통해 방향을 전환하여 벽과 닿지 않도록 한다. 
- 일정 시간 이상 나아가도 벽과 부딪히지 않은 기기는 랜덤한 방향으로 회전, 이동한다.
- 기기의 버튼을 누르면 알람이 종료된다. 혹은 사용자가 설정한 시간이 지나면 알람이 종료된다.
- LCD에는 RTC모듈을 이용해 현재 시간을 보여준다.

## 흐름도
![dataFlow](/image/flow_diagram.png)

---

## Contributors
안혜준, 정진성, 박진영, 한병정, 김명서

## License
This project is licensed under the terms of the MIT license.
