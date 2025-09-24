#ifndef __STEP_CONTROL_H__
#define __STEP_CONTROL_H__

#include <stdint.h>
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MODE_ONOFF 1
#define MODE_PID 2

typedef enum{
    IDLE = 0,
    FORWARD = 1,
    REVERSE = 2
} MotorDirection_t;

typedef enum{
    MOTOR_STATE_DISABLE = 0,
    MOTOR_STATE_ENABLE = 1
} MotorEnable_t;



//------------------------------------------
// 💠 Cấu trúc hệ thống
//------------------------------------------
typedef struct {
    uint16_t Device_ID;            // 0x0000
    uint16_t Firmware_Version;     // 0x0001
    uint16_t System_Status;        // 0x0002
    uint16_t System_Error;         // 0x0003
    uint16_t Reset_Error_Command;  // 0x0004
    uint16_t Config_Baudrate;      // 0x0005
    uint16_t Config_Parity;        // 0x0006
} SystemRegisterMap_t;

typedef struct {
    uint8_t Control_Mode;          // Base + 0x00
    uint8_t Enable;                // Base + 0x01
    uint8_t Command_Speed;         // Base + 0x02
    uint8_t Actual_Speed;          // Base + 0x03
    uint8_t Direction;             // Base + 0x04
    uint8_t Max_Speed;             // Base + 0x05
    uint8_t Min_Speed;             // Base + 0x06
    uint8_t Vmax;                  // Base + 0x07
    uint8_t Amax;                  // Base + 0x08
    uint8_t Jmax;                  // Base + 0x09
    uint8_t Max_Acc;      // Base + 0x0A
    uint8_t Max_Dec;      // Base + 0x0B
    uint8_t Status_Word;           // Base + 0x0C
    uint8_t Error_Code;           // Base + 0x0D
} MotorRegisterMap_t;

typedef struct {
    float v_target;    // velocity (steps/s)
    float a;    // acceleration (steps/s^2)
    float j;    // jerk (steps/s^3)
    float pos;  // position (steps)
    float Distance;  // total steps
    float dt;      // 1 ms
    float v_actual; // actual velocity (steps/s)
} MotionState_t;
//------------------------------------------
//  Vùng nhớ ánh xạ thanh ghi
//------------------------------------------
#define HOLDING_REG_SIZE     256
extern uint8_t modbus_holding_registers[HOLDING_REG_SIZE];

//------------------------------------------
//  Con trỏ struct ánh xạ từ vùng nhớ
//------------------------------------------

extern MotorRegisterMap_t motor1;
extern MotorRegisterMap_t motor2;

extern SystemRegisterMap_t system;

extern MotionState_t m1_motion_state;
extern MotionState_t m2_motion_state;



//------------------------------------------
//  Các hàm thao tác
//------------------------------------------

// Khởi tạo
void SystemRegisters_Init(SystemRegisterMap_t* sys);
void MotorRegisters_Init(MotorRegisterMap_t* motor);

// Load từ modbus registers
void MotorRegisters_Load(MotorRegisterMap_t* motor, uint16_t base_addr);
void SystemRegisters_Load(SystemRegisterMap_t* sys, uint16_t base_addr);

// Save lại vào modbus registers
void MotorRegisters_Save(MotorRegisterMap_t* motor, uint16_t base_addr);
void SystemRegisters_Save(SystemRegisterMap_t* sys, uint16_t base_addr);

// Xử lý logic điều khiển motor
void Motor_ProcessControl(MotorRegisterMap_t* motor);

void Motor_Set_Mode(MotorRegisterMap_t* motor, uint8_t mode);
void Motor_Set_Enable(MotorRegisterMap_t* motor);
void Motor_Set_Disable(MotorRegisterMap_t* motor);
void Motor_Set_Direction(MotorRegisterMap_t* motor, uint8_t direction);
void Motor_Set_Speed(MotorRegisterMap_t* motor, uint8_t speed);


uint8_t Motor_Get_Mode(MotorRegisterMap_t* motor);
uint8_t Motor_Get_Enable(MotorRegisterMap_t* motor);
uint8_t Motor_Get_Direction(MotorRegisterMap_t* motor);
uint8_t Motor_Get_Speed(MotorRegisterMap_t* motor);

uint8_t Motor_Get_Status_Word(MotorRegisterMap_t* motor);
uint8_t Motor_Get_Error_Code(MotorRegisterMap_t* motor);


// Xử lý ON/OFF mode (mode 1)
uint8_t Motor_HandleOnOff(MotorRegisterMap_t* motor);

// Xử lý LINEAR mode (mode 2)
// Xử lý PID mode (mode 3)
uint8_t Motor_HandleRamp(MotorRegisterMap_t* motor);

uint8_t MotionProfile_Update(MotionState_t* motion_state);

// Gửi tín hiệu PWM dựa vào Actual_Speed
void Motor1_OutputPWM(MotorRegisterMap_t* motor, uint8_t duty_percent);  // motor_id = 1 hoặc 2
void Motor2_OutputPWM(MotorRegisterMap_t* motor, uint8_t duty_percent);  // motor_id = 1 hoặc 2

void Ramp_Init(uint8_t motor_id, float vmax, float amax, float jmax);

// Điều khiển chiều quay motor
void Motor_SetDirection(uint8_t motor_id, uint8_t direction);  // 0=Idle, 1=Forward, 2=Reverse

// Reset các lỗi nếu có
void Motor_ResetError(MotorRegisterMap_t* motor);

// Kiểm tra và xử lý các điều kiện lỗi (overcurrent, timeout,...)
void Motor_CheckError(MotorRegisterMap_t* motor);

// Debug/log
void Motor_DebugPrint(const MotorRegisterMap_t* motor, const char* name);
void System_DebugPrint(const SystemRegisterMap_t* sys);

#ifdef __cplusplus
}
#endif

#endif // __MOTOR_MODBUS_MAP_H__