

---

# STM32F103 + LM35: Đo nhiệt độ và hiển thị qua UART

## Giới thiệu

Dự án này sử dụng **STM32F103C8T6** để đọc nhiệt độ từ cảm biến **LM35** thông qua **ADC 12-bit**, sau đó tính toán điện áp và truyền kết quả qua cổng UART để hiển thị trên máy tính.

---

## Phần cứng

* **Vi điều khiển:** STM32F103C8T6 (Blue Pill)
* **Cảm biến nhiệt độ:** LM35
* **Nguồn:** 5 V hoặc 3.3 V (nên dùng 5 V cho LM35)
* **Kết nối UART:** USB-TTL (PA9: TX, PA10: RX)

### Sơ đồ nối cơ bản

| LM35 | STM32          |
| ---- | -------------- |
| VCC  | 5 V            |
| GND  | GND            |
| VOUT | PA0 (ADC1_IN0) |

> Nếu dùng chân ADC khác, cần đổi kênh trong code (`ADC_Channel_X`).

---

## Phần mềm

* **Ngôn ngữ:** C
* **Toolchain:** Keil uVision / STM32 Standard Peripheral Library
* **Tốc độ UART:** 9600 bps (có thể chỉnh trong hàm `UART1_Init`)

---

## Chức năng chính

1. Khởi tạo UART1 để gửi dữ liệu sang máy tính.
2. Cấu hình ADC1 kênh 0 để đọc điện áp từ LM35.
3. Chuyển đổi giá trị ADC (0–4095) sang điện áp (mV) theo công thức:

   ```
   V(mV) = ADC_value / 4095 * 3300
   ```
4. Tính nhiệt độ (°C):

   ```
   Temperature = V(mV) / 10
   ```
5. Gửi kết quả “ADC = …, Voltage = … mV” qua UART mỗi 500 ms.
   
   ```
   ADC=2050  Voltage=1650.00 mV
   ```

---




