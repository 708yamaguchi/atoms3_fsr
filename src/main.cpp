#define LGFX_M5ATOMS3
#define LGFX_USE_V1
#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX.hpp>

static LGFX lcd;
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define BLACK 0x0000

#include <Wire.h>
#include "ADS7828.h"
ADS7828 ads[2] = {ADS7828(0x48), ADS7828(0x49)};
const int NCHANNELS = 8;

#define NUM_POINTS 10
// data points: {Force[g], Resistance[kΩ]}
static const float force_resistance_data[NUM_POINTS][2] = {
    {20.0, 100.0},
    {50.0, 40.0},
    {100.0, 20.0},
    {200.0, 10.0},
    {400.0, 5.0},
    {600.0, 3.5},
    {800.0, 2.8},
    {1000.0, 2.4},
    {1500.0, 1.8},
    {2000.0, 1.5}};

int adc_values[2][NCHANNELS];
float fsr_registance[2][NCHANNELS];

void readADC() {
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < NCHANNELS; i++) {
      adc_values[j][i] = ads[j].getValue(i);
    }
  }
}

void convertToRegistance() {
  float vout;
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < NCHANNELS; i++) {
      // If you use internal refernce voltage, set 2.5[V]
      float vref = 2.5;
      vout = vref * adc_values[j][i] / 4096.0;
      fsr_registance[j][i] = 22.4 / vout - 6.8;
    }
  }
}

float RegistanceToForce(float resistance_kOhm) {
    // 入力抵抗値が既知の最大抵抗値以上の場合 (グラフの左側)
    // force_resistance_data[0][1] が最大の抵抗値
    if (resistance_kOhm >= force_resistance_data[0][1]) {
        return 0;
    }

    // 入力抵抗値が既知の最小抵抗値以下の場合 (グラフの右側)
    // force_resistance_data[NUM_POINTS - 1][1] が最小の抵抗値
    if (resistance_kOhm <= force_resistance_data[NUM_POINTS - 1][1]) {
        return -1;
    }

    // 適切な2点間を見つけて線形補間を行う
    for (int i = 0; i < NUM_POINTS - 1; ++i) {
        float R_upper = force_resistance_data[i][1];     // 区間の上限抵抗値 R_i
        float F_at_R_upper = force_resistance_data[i][0]; // R_i での力 F_i

        float R_lower = force_resistance_data[i + 1][1];   // 区間の下限抵抗値 R_{i+1}
        float F_at_R_lower = force_resistance_data[i + 1][0]; // R_{i+1} での力 F_{i+1}

        // 入力抵抗値が現在の区間 [R_lower, R_upper] 内にあるか確認
        // 配列は抵抗値の降順なので R_upper >= resistance_kOhm >= R_lower
        if (resistance_kOhm <= R_upper && resistance_kOhm >= R_lower) {
            // データポイントに完全に一致する場合
            if (resistance_kOhm == R_upper) {
                return F_at_R_upper;
            }
            if (resistance_kOhm == R_lower) {
                return F_at_R_lower;
            }

            // 線形補間の計算
            // F = F1 + (F2 - F1) * (R_in - R1) / (R2 - R1)
            // ここで、(R1, F1) = (R_upper, F_at_R_upper)
            //        (R2, F2) = (R_lower, F_at_R_lower)
            //        R_in = resistance_kOhm
            float interpolated_force = F_at_R_upper + (F_at_R_lower - F_at_R_upper) *
                                       (resistance_kOhm - R_upper) / (R_lower - R_upper);
            return interpolated_force;
        }
    }
    return -1;
}

void updateLCD() {
  lcd.startWrite();
  lcd.fillScreen(BLACK);
  lcd.setCursor(0, 0);
  lcd.setTextSize(1);
  lcd.setTextColor(WHITE, BLACK);
  lcd.println(" Force calculation\n");
  lcd.println(" ADDR  0x48 | 0x49 ");
  lcd.println("--------------------");

  int total_force = 0;
  // Draw individual force
  for (int i = 0; i < NCHANNELS; i++) {
    lcd.setTextColor(WHITE, BLACK);
    lcd.printf(" CH%d   ", i);
    int force1 = (int)RegistanceToForce(fsr_registance[0][i]);
    int force2 = (int)RegistanceToForce(fsr_registance[1][i]);
    lcd.setTextColor(GREEN, BLACK);
    if (force1 == -1) {
        lcd.printf("%4s", "MAX");
        total_force = -1;
    } else {
        lcd.printf("%4d", force1);
        if (total_force != -1) {
            total_force += force1;
        }
    }
    lcd.setTextColor(WHITE, BLACK);
    lcd.print(" | ");
    lcd.setTextColor(GREEN, BLACK);
    if (force2 == -1) {
        lcd.printf("%4s\n", "MAX");
        total_force = -1;
    } else {
        lcd.printf("%4d\n", force2);
        if (total_force != -1) {
            total_force += force2;
        }
    }
  }

  // Draw total force
  lcd.setTextSize(1.5);
  lcd.setTextColor(WHITE, BLACK);
  lcd.print("\nTotal ");
  lcd.setTextColor(GREEN, BLACK);
  if (total_force == -1) {
    lcd.printf("%5s", "MAX");
  } else {
    lcd.printf("%5d", total_force);
  }
  lcd.setTextColor(WHITE, BLACK);
  lcd.println("[g]");

  lcd.endWrite();
}

void setup() {
  lcd.init();
  lcd.setRotation(3);
  lcd.setTextSize(1);
  lcd.setTextColor(GREEN, BLACK);
  lcd.println("FSR Reader Initialized");
  Wire.begin(2, 1);
  delay(100);
}

void loop() {
  readADC();
  convertToRegistance();
  updateLCD();
  delay(100);
}
