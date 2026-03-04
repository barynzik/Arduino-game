#define Taster D2   

#include <SPI.h>                     // Einbinden der zentralen Grafik-Library   
#include <TFT_eSPI.h>                // Einbinden der Library für die Hardware   

TFT_eSPI tft = TFT_eSPI();           // aufrufen der Libary für die Hardware   

const int maxx = 240;                // Größe des Displays   
const int maxy = 320;

const int maxbaelle = 3;             // maximale Anzahl der Bälle   


const int ZielX = 25;                // Koordinaten Zielfeld   
const int ZielY = 25;
const int ZielBreite = 190;
const int ZielHoehe = 270;

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

void ZufallsBaelle()                 //создает шарик 
{
    Ball[0].d = 9;
    Ball[1].d = 7;
    Ball[2].d = 4;
    for (int i = 0; i < maxbaelle; i++) {          // и = 0; не больше 3 и увеличивается на 1 (счетчик того, сколько условия которые в фор луп повторятся) 
        // теперь шарик в любом случае будет в границах экрана  
        Ball[i].x = random(Ball[i].d, maxx - Ball[i].d);   // случайня позиция по х и у 
        Ball[i].y = random(Ball[i].d, maxy - Ball[i].d);

        Ball[i].gx = ballSpeed * (random(2) ? 1 : -1);      // направленте скоросьт по х и у; random 2 gives 0 or 2  решает направление 
        Ball[i].gy = ballSpeed * (random(2) ? 1 : -1);

        Ball[i].px = Ball[i].x;                 // текушая позиция  на экране;  шар "стирается" и не виден человеческому глазуб т потом он появляется снова  на новой позиции 
        Ball[i].py = Ball[i].y;
    }
}

void SpielfeldZeichnen() {
    tft.drawRect(ZielX, ZielY, ZielBreite, ZielHoehe, TFT_RED); // Spielfeld zeichnen   
}

// новая функция для обновления мячика  
void UpdateBaelle() {
    for (int i = 0; i < maxbaelle; i++) {        //  с каждим шарлм проимходит  

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

//void BaelleZeichnen() { 
   // for (int i = 0; i < maxbaelle; i++) 
   // { 
 //       tft.fillCircle((int)Ball[i].x, (int)Ball[i].y, Ball[i].d, TFT_YELLOW); 
   // } 
//} 
// теперь функция чтобы проверить отсутствуют ли шарики в квадратике  

bool BallCheck() {           //   проверяет местоположение мяча; функция всегда видит его в квадрате  если его нет его нет 
    for (int i = 0; i < maxbaelle; i++) {
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
    for (int i = 0; i < maxbaelle; i++)
    {
        Ball[i].gx = ballSpeed * (Ball[i].gx >= 0 ? 1 : -1);
        Ball[i].gy = ballSpeed * (Ball[i].gy >= 0 ? 1 : -1);
    }
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
            tft.drawString("LEVEL UP!", maxx / 2, maxy / 2);

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
            tft.drawString("GAME OVER!", maxx / 2, maxy / 2);
            delay(1000);
            ballSpeed = 3.0;
            RestartGame();
            SpielfeldZeichnen();
        }

        while (digitalRead(Taster) == LOW) delay(10);
    }

    delay(geschwindigkeit);
}