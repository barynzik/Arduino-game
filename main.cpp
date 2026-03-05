﻿#define Taster D2

#include <SPI.h>                     // Einbinden der zentralen Grafik-Library   
#include <TFT_eSPI.h>                // Einbinden der Library für die Hardware   

TFT_eSPI tft = TFT_eSPI();           // aufrufen der Libary für die Hardware   

const int maxx = 240;                // Größe des Displays   
const int maxy = 320;

const int maxbaelle = 5;             // maximale Anzahl der Bälle   
int activebaelle = 3;

int ZielX = 25;
int ZielY = 25;
int ZielBreite = 190;
int ZielHoehe = 270;

int geschwindigkeit = 25;           // Wartezeit zwischen den Aktualisierungsintervallen   
int level = 1;                      //переменая левел увеличиваетсятна 1 
float  ballSpeed = 3;               // скорость мячей 

struct strukturBall                 //описание, параметри шара 
{
    float  x, y;
    int px, py;     // ← добавили 
    int d;
    float  gx, gy;
};

strukturBall Ball[maxbaelle];        // Array mit den Bällen wird angelegt   

// перенес void setup вниз чтобы вызвать там ZufallsBaelle();;  

void InitBall(int i)
{
    int ds[5] = { 9, 7, 4, 6, 5 };
    Ball[i].d = ds[i];

    Ball[i].x = random(Ball[i].d, maxx - Ball[i].d);
    Ball[i].y = random(Ball[i].d, maxy - Ball[i].d);

    Ball[i].gx = ballSpeed * (random(2) ? 1 : -1);
    Ball[i].gy = ballSpeed * (random(2) ? 1 : -1);

    Ball[i].px = (int)Ball[i].x;
    Ball[i].py = (int)Ball[i].y;
}

void ZufallsBaelle()
{
    for (int i = 0; i < activebaelle; i++) {
        InitBall(i);
    }
}

void SpielfeldZeichnen() {
    tft.drawRect(ZielX, ZielY, ZielBreite, ZielHoehe, TFT_RED); // Spielfeld zeichnen   
}

// новая функция для обновления мячика  
void UpdateBaelle() {
    for (int i = 0; i < activebaelle; i++) {        //  с каждим шарлм проимходит  

        // стереть старый шарик 
        tft.fillCircle(Ball[i].px, Ball[i].py, Ball[i].d, TFT_BLACK);

        // сохранить старую позицию 
        Ball[i].px = Ball[i].x;
        Ball[i].py = Ball[i].y;

        // движение 
        Ball[i].x += Ball[i].gx; // х и у меняются на скорость 
        Ball[i].y += Ball[i].gy;

        if (Ball[i].x - Ball[i].d <= 0 || Ball[i].x + Ball[i].d >= maxx)     // шарик меняет направление скорости; ударился в екран ; летит в другую сторону  
            Ball[i].gx = -Ball[i].gx;

        if (Ball[i].y - Ball[i].d <= 0 || Ball[i].y + Ball[i].d >= maxy)
            Ball[i].gy = -Ball[i].gy;

        // нарисовать новый шарик 
        tft.fillCircle((int)Ball[i].x, (int)Ball[i].y, Ball[i].d, TFT_YELLOW);
    }
}
// теперь функция чтобы проверить отсутствуют ли шарики в квадратике  

void ShrinkField() 
{
    int dx = ZielBreite / 8;
    int dy = ZielHoehe / 8;

    ZielX += dx;
    ZielY += dy;

    ZielBreite -= (ZielBreite / 4);
    ZielHoehe  -= (ZielHoehe / 4);
}

void AddBallIfPossible() {
    if (activebaelle < maxbaelle) {
        InitBall(activebaelle);
        activebaelle++;
    }
}

void ApplyLevelRules() {
    if (level == 3)  ShrinkField();
    if (level == 5)  AddBallIfPossible();
    if (level == 7)  ShrinkField();
    if (level == 9)  AddBallIfPossible();
    if (level == 11) ShrinkField();
}

bool BallCheck() {           //   проверяет местоположение мяча; функция всегда видит его в квадрате  если его нет его нет 
    for (int i = 0; i < activebaelle; i++) {
        if (Ball[i].x - Ball[i].d < ZielX) return false;
        if (Ball[i].x + Ball[i].d > ZielX + ZielBreite) return false;
        if (Ball[i].y - Ball[i].d < ZielY) return false;
        if (Ball[i].y + Ball[i].d > ZielY + ZielHoehe) return false;
    }
    return true;
}

void setup() {
    pinMode(Taster, INPUT_PULLUP);
    tft.init();
    tft.setRotation(2);
    tft.fillScreen(TFT_BLACK);

    SpielfeldZeichnen();
    ZufallsBaelle();
}
void RestartGame()                   //  начинает игру сначала 
{
    tft.fillScreen(TFT_BLACK);
    ZufallsBaelle();
}
void LevelUp()                  //  увеличение переменной левел 
{
    level++;
    ballSpeed += 0.3;
    for (int i = 0; i < activebaelle; i++)
    {
        Ball[i].gx = ballSpeed * (Ball[i].gx >= 0 ? 1 : -1);
        Ball[i].gy = ballSpeed * (Ball[i].gy >= 0 ? 1 : -1);
    }
    ApplyLevelRules();
}

void GameOver()
{
    level = 1;
    ballSpeed = 3.0;
    activebaelle = 3;
    ZielBreite = 190;
    ZielHoehe = 270;
    ZielX = 25;
    ZielY = 25;
}

void loop() {

    SpielfeldZeichnen();
    UpdateBaelle();

    if (digitalRead(Taster) == LOW)
    {
        if (BallCheck()) {

            tft.fillScreen(TFT_GREEN);
            tft.setTextColor(TFT_WHITE, TFT_GREEN);
            tft.setTextSize(3);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("LEVEL UP!", tft.width() / 2, tft.height() / 2);

            delay(2000);

            LevelUp();
            RestartGame();
            SpielfeldZeichnen();
        }
        else {
            tft.fillScreen(TFT_RED);
            tft.setTextColor(TFT_WHITE, TFT_RED);
            tft.setTextSize(3);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("GAME OVER!", tft.width() / 2, tft.height() / 2);
            delay(1000);
            GameOver();
            RestartGame();
            SpielfeldZeichnen();
        }

        while (digitalRead(Taster) == LOW) delay(10);
    }

    delay(geschwindigkeit);
}