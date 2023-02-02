#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define WIDTH 30
#define HEIGHT 20

//Czas w MS
#define EASY 200
#define MEDIUM 150
#define HARD 100

int snake_x, snake_y, food_x, food_y;
int last_snake_x, last_snake_y, last_tail_x, last_tail_y;
int tail_x[250], tail_y[250], tail_length, debug = 0;
int direction, score, last_score, game_over, gra, przeszkody, s_time;
int wyjscie, menu_enter, menu_opcja, poziom_trudnosci = 0;
int obstacle_x[32], obstacle_y[32], fcoords[2];

typedef struct wyniki{
char nazwa[20];
int wynik;
} Wyniki;

Wyniki gracze[3];


typedef struct Menu {
    char mainMENU[4][30];
    int sizeMENU;
} Menu;

void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void SetColor(int textC,int backgroundC) {
    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
    {
        wColor = (backgroundC*16 & 0xF0) + (textC & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    }
}

void init_obstacles() {
    int i;
    for(i=0;i<6;i++) {
        obstacle_x[i]=11;
        obstacle_y[i]=3+i;
    }
    int j=0;
    for(i=6;i<16;i++) {
        obstacle_x[i]=23+j;
        obstacle_y[i]=11;
        j++;
    }
    j=0;
    for(i=16;i<19;i++) {
        obstacle_x[i]=8;
        obstacle_y[i]=16+j;
        j++;
    }
    j=0;
    for(i=19;i<32;i++) {
        obstacle_x[i]=8+j;
        obstacle_y[i]=19;
        j++;
    }
}

void logo() {
    FILE * F = fopen("logo.txt","r");
    char c;
    int ile=0;
    gotoxy(40,1);
    if(F!=NULL) {
        while(!feof(F)) {
            c = getc(F);
            if(c==10) {
                printf("\n");
                ile++;
                gotoxy(40,1+ile);
            } else {
                printf("%c",c);
            }
        }
    } else {
        printf("Blad pliku");
    }
}

void menu(Menu M,int opcja,int poziom_trudnosci) {
    gotoxy(56,7);
    int ile=0;
    printf("| MENU |\n");
    int i;
    for(i=0;i<M.sizeMENU;i++) {
        ile++;
        if(opcja==i) {
            gotoxy(48,8+ile);
            printf(">  %s",M.mainMENU[i]);
            printf("\n");
        } else {
            gotoxy(48,8+ile);
            printf("%s",M.mainMENU[i]);
            if(i==1) {
                switch(poziom_trudnosci) {
                    case 0: {
                        SetColor(10,0);
                        printf("latwy");
                        SetColor(7,0);
                    } break;
                    case 1: {
                        SetColor(6,0);
                        printf("sredni");
                        SetColor(7,0);
                    } break;
                    case 2: {
                        SetColor(4,0);
                        printf("trudny");
                        SetColor(7,0);
                    } break;
                }
            }
            printf("\n");
        }

    }
}

void menu_zmiana(Menu M,int nowa_opcja,int stara_opcja,int poziom_trudnosci) {
    int i;
    int x1=(120-strlen(M.mainMENU[stara_opcja])+2)/2;
    int x2=(120-strlen(M.mainMENU[nowa_opcja]))/2;
    gotoxy(0,9+stara_opcja);
    for(i=0;i<120;i++) { printf(" ");}
    gotoxy(48,9+stara_opcja);
    printf("%s",M.mainMENU[stara_opcja]);
    if(stara_opcja==1) {
        switch(poziom_trudnosci) {
            case 0: {
                SetColor(10,0);
                printf("latwy");
                SetColor(7,0);
            } break;
            case 1: {
                SetColor(6,0);
                printf("sredni");
                SetColor(7,0);
            } break;
            case 2: {
            SetColor(4,0);
            printf("trudny");
            SetColor(7,0);
            } break;
        }
    }
    printf("\n");
    gotoxy(0,9+nowa_opcja);
    for(i=0;i<120;i++) { printf(" ");}
    gotoxy(48,9+nowa_opcja);
    printf(">  %s",M.mainMENU[nowa_opcja]);
    if(nowa_opcja==1) {
        switch(poziom_trudnosci) {
            case 0: {
                SetColor(10,0);
                printf("latwy");
                SetColor(7,0);
            } break;
            case 1: {
                SetColor(6,0);
                printf("sredni");
                SetColor(7,0);
            } break;
            case 2: {
            SetColor(4,0);
            printf("trudny");
            SetColor(7,0);
            } break;
        }
    }
    printf("\n");
}

void sterowanie() {
    gotoxy(5,17);
    printf("STEROWANIE:\n");
    printf("\tENTER - wybierz opcje\n");
    printf("\t%c - sterowanie w gore\n",24);
    printf("\t%c - sterowanie w dol\n",25);
    printf("\t< - sterowanie w lewo\n");
    printf("\t> - sterowanie w prawo\n");
}

void poziomy_trudnosci() {
    gotoxy(5,24);
    printf("POZIOMY TRUDNOSCI:\n");
    SetColor(10,0);
    printf("\tLatwy ");
    SetColor(7,0);
    printf(" - Mozliwosc przenikania przez sciany, waz porusza sie wolno\n");
    SetColor(6,0);
    printf("\tSredni ");
    SetColor(7,0);
    printf(" - Brak mozliwosc przenikania przez sciany, waz porusza sie szybko\n");
    SetColor(4,0);
    printf("\tTrudny ");
    SetColor(7,0);
    printf(" - Brak mozliwosc przenikania przez sciany, waz porusza sie bardzo szybko, \n\tplansza zawiera dodatkowe przeszkody\n");

}

void randomcoords(int przeszkody) {
    while (1) {
        int x = rand() % 30 + 3; 
        int y = rand() % 20 + 3; 
        int f = 1; 
        int j,h;
        if(x==snake_x && y==snake_y) {
            f=0;
            break;
        } else {
            for(j=0;j<tail_length;j++) {
                if(tail_x[j]==x && tail_y[j]==y) {
                    f=0;
                    break;
                } 
            }
            if(przeszkody) {
                for(h = 0; h < 32; h++) {
                    if (obstacle_x[h] == x && obstacle_y[h] == y) {
                        f = 0;
                        break;
                    }
                }
            }
        }
        if (f) {
            fcoords[0] = x;
            fcoords[1] = y;
            break;
        }
    }
}


void init_snake() {
    snake_x = (WIDTH+4)/2;
    snake_y = (HEIGHT+4)/2;
    randomcoords(przeszkody);
    food_x=fcoords[0];
    food_y=fcoords[1];
    tail_length = 0;
    direction = 0;
    score = 0;
}

void printAt(int x, int y, char c) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("%c", c);
}

void wczytaj_wyniki() {
    FILE*F=fopen("wyniki.txt","r");
    if(F==NULL) {
        printf("blad odczytu pliku\n");
    } else {
        int i=0;
        while(fscanf(F,"%s %d", gracze[i].nazwa, &gracze[i].wynik) == 2) {
            if(strlen(gracze[i].nazwa) <1) {
                gracze[i].nazwa == "Brak";
            }
            i++;
        }
        fclose(F);
    }
}

int najlepszy_wynik(int poziom_trudnosci) {
    return gracze[poziom_trudnosci].wynik;
}

void update_wynik(int poziom_trudnosci,int wynik, char nick[20]) {
    FILE*H=fopen("wyniki.txt","w");
    if(H!=NULL) {
        int i;
        for(i=0;i<3;i++) {
            if(i==poziom_trudnosci) {
                fprintf(H,"%s %d\n",nick,wynik);
            } else {
                if(strlen(gracze[i].nazwa) <1) {
                    fprintf(H,"brak %d\n",gracze[i].wynik);
                } else {
                    fprintf(H,"%s %d\n",gracze[i].nazwa,gracze[i].wynik);
                }
            }
        }
    }
    fclose(H);
}

void print_przeszkody(int symbol) {
    if(przeszkody) {
        int i;
        for(i=0;i<32;i++) {
            printAt(obstacle_x[i],obstacle_y[i],symbol);
        }
    }
}

void plansza(int width,int height) {
    int i;
    for(i=2;i<width+4;i++) {
        gotoxy(i,2);
        printf("%c",219);
        gotoxy(i,height+3);
        printf("%c",219);
    }
    for(i=3;i<height+3;i++) {
        gotoxy(2,i);
        printf("%c",219);
        gotoxy(width+3,i);
        printf("%c",219);
    }
    print_przeszkody(219);
}

void wyswietl_wyniki() {
        gotoxy(52,7);
        printf("| NAJLEPSZE WYNIKI |\n");
        gotoxy(48,9);
        SetColor(10,0);
        printf("Latwy\t\t");
        SetColor(7,0);
        if(strlen(gracze[0].nazwa) <1 || (strcmp(gracze[0].nazwa,"brak")==0 && gracze[0].wynik==0)) {
            printf("Brak");
        } else {
            printf("%s: %d",gracze[0].nazwa, gracze[0].wynik);
        }
        gotoxy(48,11);
        SetColor(6,0);
        printf("Sredni\t\t");
        SetColor(7,0);
        if(strlen(gracze[1].nazwa) <1 || (strcmp(gracze[1].nazwa,"brak")==0 && gracze[1].wynik==0)) {
            printf("Brak");
        } else {
            printf("%s: %d",gracze[1].nazwa, gracze[1].wynik);
        }
        gotoxy(48,13);
        SetColor(4,0);
        printf("Trudny\t\t");
        SetColor(7,0);
        if(strlen(gracze[2].nazwa) <1 || (strcmp(gracze[2].nazwa,"brak")==0 && gracze[2].wynik==0)) {
            printf("Brak");
        } else {
            printf("%s: %d",gracze[2].nazwa, gracze[2].wynik);
        }
}

void igame_over() {
    game_over = 1;
    last_score=score;
    init_snake();
}

void update(int poziom_trudnosci) {
    //kolizja z przeszkoda;
    if(przeszkody) {
        int i;
        for(i=0;i<32;i++) {
            if(snake_x==obstacle_x[i] && snake_y==obstacle_y[i]) {
                igame_over();
            }
        }
    }

    if(score>najlepszy_wynik(poziom_trudnosci)) {
        SetColor(14,0);
        gotoxy(47,9);
        printf("     ");
        gotoxy(47,9);
        printf("%d",score);
    } else {
        SetColor(7,0);
        gotoxy(47,9);
        printf("     ");
        gotoxy(47,9);
        printf("%d",score);
    }
    
    if(!game_over) {
       // if(last_snake_x!=2 && last_snake_y!=2) {
            if(last_snake_x!=0) {
                SetColor(8,0);
                printAt(last_snake_x, last_snake_y, '.');
                SetColor(7,0);
            }
       // }
        if(tail_length>0) {
            if(last_tail_x!=0) {
                SetColor(8,0);
                printAt(last_tail_x,last_tail_y,'.');
                SetColor(7,0);
            }
                
        }
        SetColor(4,0);
        printAt(food_x, food_y, 3);
        SetColor(2,0);
       // if(snake_x!=2 && snake_y!=2) {
            printAt(snake_x, snake_y, 'O');
        //}
        int i;
        for (i = 0; i < tail_length; i++) {
            last_tail_x = tail_x[i];
            last_tail_y = tail_y[i];
            if(last_tail_x==last_snake_x) {
                printAt(tail_x[i], tail_y[i], 'o');
            }
        }
    }
    //kolizja ze sciana
    if(poziom_trudnosci>0) {
        if((snake_x==3 && direction==3) || (snake_x==32 && direction==1) || (snake_y==3 & direction==0) || (snake_y==22 && direction==2)) {
            igame_over();
        }
    }
    SetColor(7,0);
    if(debug) {
        gotoxy(80,7);
        printf("                    ");
        gotoxy(80,7);
        printf("FOOD X=%d Y=%d",food_x,food_y);
        gotoxy(80,8);
        printf("                    ");
        gotoxy(80,8);
        printf("HEAD X=%d Y=%d",snake_x,snake_y);
        gotoxy(80,9);
        printf("                    ");
        gotoxy(80,9);
        printf("LAST_HEAD X=%d Y=%d",last_snake_x,last_snake_y);
        gotoxy(80,10);
        printf("Direction: %d",direction);
        gotoxy(80,11);
        printf("                    ");
        gotoxy(80,11);
        printf("TAIL_LENGTH=%d",tail_length);
        gotoxy(80,12);
        printf("                    ");
        gotoxy(80,12); 
        printf("LAST_TAIL X=%d Y=%d",last_tail_x,last_tail_y);
        gotoxy(80,13);
        printf("                    ");
        gotoxy(80,13);
        printf("TAIL[L] X=%d Y=%d",tail_x[tail_length],tail_y[tail_length]);
        gotoxy(80,14);
        printf("                    ");
        gotoxy(80,14);
        printf("TAIL[0] X=%d Y=%d",tail_x[0],tail_y[0]);
        gotoxy(80,15);
        printf("                    ");
        gotoxy(80,15);
        printf("S_TIME: %dMS",s_time);
        gotoxy(80,16);
        printf("                    ");
        gotoxy(80,16);
        printf("OBSTACLES: %d",przeszkody);
    }
}

void input() {
    if (kbhit()) {
        char c = getch();
        switch (c) {
            case -32: {
                c = getch();
                switch(c) {
                    case 72: {
                        if (direction != 2) {
                            direction = 0;
                        }
                    } break;
                    case 77: {
                        if (direction != 3) {
                            direction = 1;
                        }
                    } break;
                    case 80: {
                        if (direction != 0) {
                            direction = 2;
                        }
                    } break;
                    case 75: {
                        if (direction != 1) {
                            direction = 3;
                        }
                    } break;
                }
            } break;
            case 27: {
                last_score=score;
                gra = 0;
                game_over = 1;
                wyjscie = 1;
                menu_enter=0;
                menu_opcja=0;
            } break;
        }
    }
}

void move() {
    int i;
    for (i = tail_length - 1; i >= 1; i--) {
        tail_x[i] = tail_x[i - 1];
        tail_y[i] = tail_y[i - 1];
    }
    tail_x[0] = snake_x;
    tail_y[0] = snake_y;
    last_snake_x = snake_x;
    last_snake_y = snake_y;
    switch (direction) {
        case 0: {
            last_snake_y = snake_y;
            snake_y--;
        } break;
        case 1: {
            last_snake_x = snake_x;
            snake_x++;
        } break;
        case 2: {
            last_snake_y = snake_y;
            snake_y++;
        } break;
        case 3: {
            last_snake_x = snake_x;
            snake_x--;
        } break;
    }

    if (snake_x < 3) {
        snake_x = WIDTH+3 - 1;
    }
    if (snake_x >= WIDTH+3) {
        snake_x = 3;
    }
    if (snake_y < 3) {
     snake_y = HEIGHT+3 - 1;
    }
    if (snake_y >= HEIGHT+3) {
        snake_y = 3;
    }

    for (i = 0; i < tail_length; i++) {
        if (tail_x[i] == snake_x && tail_y[i] == snake_y) {
            igame_over();
        }
    }
    if (snake_x == food_x && snake_y == food_y) {
        tail_length++;
        score += 1;
        randomcoords(przeszkody);
        food_x=fcoords[0];
        food_y=fcoords[1];
    }
}

void wyczysc_plansze() {
    int i,j;
    for(i=3;i<33;i++) {
        for(j=3;j<23;j++) {
            SetColor(8,0);
            printAt(i,j,'.');
            SetColor(7,0);
        }
    }
    print_przeszkody(219);
}

int main() {
    SetConsoleTitle("SNAKE");
    srand(time(NULL));
    init_obstacles();
    wczytaj_wyniki();
    printf("\e[?25l"); //ukrywanie kursora
    int menu_opcja = 0;
    int menu_opcja1 = 0;
    int menu_enter = 0;
    int koniec = 0;
    gra = 0;
    int wyniki= 0;
    char c = ' ';

    Menu M;
    strcpy(M.mainMENU[0],"Zacznij gre");
    strcpy(M.mainMENU[1],"Poziom trudnosci: ");
    strcpy(M.mainMENU[2],"Najlepsze wyniki");
    strcpy(M.mainMENU[3],"Zamknij");
    M.sizeMENU=4;

    while(!koniec) {
        system("CLS");
        logo();
        menu(M,menu_opcja,poziom_trudnosci);
        sterowanie();
        poziomy_trudnosci();
        while(!menu_enter) {
            while (!_kbhit());
            c = getch();
            if(c==13) {
                if(menu_opcja!=1) {
                menu_enter = 1;
                }
            } else {
                c = getch();
                switch(c) {
                    case 72: {
                        menu_opcja1=menu_opcja;
                        if(menu_opcja==0) {
                            menu_opcja=M.sizeMENU-1;
                        } else {
                            menu_opcja--;
                        }
                        menu_zmiana(M,menu_opcja,menu_opcja1,poziom_trudnosci);
                    } break;
                    case 80: {
                        menu_opcja1=menu_opcja;
                        if(menu_opcja==3) {
                            menu_opcja=0;
                        } else {
                            menu_opcja++;
                        }
                        menu_zmiana(M,menu_opcja,menu_opcja1,poziom_trudnosci);
                    } break;
                    case 77: {
                        if(menu_opcja==1) {
                            if(poziom_trudnosci==2) {
                                poziom_trudnosci=0;
                                menu_zmiana(M,menu_opcja,menu_opcja1,poziom_trudnosci);
                            } else {
                                poziom_trudnosci++;
                                menu_zmiana(M,menu_opcja,menu_opcja1,poziom_trudnosci);
                            }
                        }
                    } break;
                    case 75: {
                        if(menu_opcja==1) {
                            if(poziom_trudnosci==0) {
                                poziom_trudnosci = 2;
                                menu_zmiana(M,menu_opcja,menu_opcja1,poziom_trudnosci);
                            } else {
                                poziom_trudnosci--;
                                menu_zmiana(M,menu_opcja,menu_opcja1,poziom_trudnosci);
                            }
                        }
                    } break;
                }
            }
        }
        switch(menu_opcja) {
            case 0: {
                gra = 1;
                menu_enter = 0;
                wyniki = 0;
            } break;
            case 2: {
                gra = 0;
                menu_enter = 0;
                wyniki = 1;
            } break;
            case 3: {
                gra = 0;
                menu_enter = 0;
                wyniki = 0;
                koniec = 1;
            } break;
        }
        while(gra) {
            wyjscie = 0;
            system("CLS");
            logo();
            gotoxy(30,16);
            gotoxy(40,7);
            printf("Poziom trudnosci: ");
            switch(poziom_trudnosci) {
                case 0: {
                    SetColor(10,0);
                    printf("LATWY");
                    SetColor(7,0);
                    przeszkody=0;
                    s_time=EASY;
                } break;
                case 1: {
                    SetColor(6,0);
                    printf("SREDNI");
                    SetColor(7,0);
                    przeszkody=0;
                    s_time=MEDIUM;
                } break;
                case 2: {
                    SetColor(4,0);
                    printf("TRUDNY");
                    SetColor(7,0);
                    przeszkody=1;
                    s_time=HARD;
                } break;
            }
            plansza(30,20);
            gotoxy(40,9);
            printf("WYNIK: %d",score);
            gotoxy(40,11);
            printf("NAJLEPSZY WYNIK: %d",najlepszy_wynik(poziom_trudnosci));
            gotoxy(40,23);
            printf("ESC - Powrot do menu");
            while(!wyjscie) {
                wyczysc_plansze();
                init_snake();
                last_score=0;
                gotoxy(40,13);
                printf("                                                           ");
                while(!game_over) {
                    input();
                    update(poziom_trudnosci);
                    move();
                    Sleep(s_time);
                }
                while(game_over) {
                    gotoxy(13,12);
                    SetColor(4,0);
                    printf("GAME OVER!");
                    SetColor(7,0);
                    gotoxy(6,13);
                    printf("Nacisnij dowolny klawisz");
                    if(last_score>najlepszy_wynik(poziom_trudnosci)) {
                        gotoxy(8,15);
                        SetColor(14,0);
                        printf("Nowy najlepszy wynik!");
                        gotoxy(7,16);
                        printf("Podaj nick aby zapisac");
                        int zapisznick=0;
                        while(!zapisznick) {
                            gotoxy(40,13);
                            SetColor(7,0);
                            printf("\e[?25h");
                            printf("PODAJ NICK: ");
                            char nick[20];
                            scanf("%s",nick);
                            zapisznick=1;
                            update_wynik(poziom_trudnosci,last_score,nick);
                            wczytaj_wyniki();
                            printf("\e[?25l");
                        }
                        gotoxy(40,11);
                        SetColor(7,0);
                        printf("NAJLEPSZY WYNIK: %d",najlepszy_wynik(poziom_trudnosci));
                    }
                    while(!_kbhit()) {
                        gotoxy(13,12);
                        SetColor(4,0);
                        printf("GAME OVER!");
                        SetColor(7,0);
                        Sleep(200);
                        gotoxy(13,12);
                        printf("GAME OVER!");
                        Sleep(200);
                    }
                    game_over = 0;
                }
                while(!_kbhit());
                char d = getch();
                if(d==27) {
                    gra = 0;
                    game_over = 0;
                    wyjscie =1;
                    wyniki = 0;
                    menu_enter=0;
                    menu_opcja=0;
                }
            }
        }
        while(wyniki) {
            int wyjscie = 0;
            system("CLS");
            logo();
            wyswietl_wyniki();
            gotoxy(40,16);
            printf("ESC - Powrot do menu");
            while(!wyjscie) {
                while(!_kbhit());
                char d = getch();
                if(d==27) {
                    gra = 0;
                    wyjscie =1;
                    wyniki = 0;
                    menu_enter=0;
                    menu_opcja=2;
                }
            }
        }
    }
    system("CLS");
    return 0;
}