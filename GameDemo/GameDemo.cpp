// GameDemo.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "GameDemo.h"

#define MAX_LOADSTRING 100
#define Wnd_width 1500
#define Wnd_height 900
#define Wnd_initx 200
#define Wnd_inity 100
#define Snow_Number 200


//前向声明
struct Snow
{
    int pos_x, pos_y;
    const int size_x = 30, size_y = 30;
    BOOL exist;
};

class SwordBullets
{
public:
    SwordBullets();
    ~SwordBullets();
    int posx, posy;
    int xsize, ysize;

    BOOL exist;
    HBITMAP bitmap;

   
};

//人物声明
class Character
{
public:
    Character();
    ~Character();
    int direction;
    HBITMAP bitmap[4];
    int xsize;
    int ysize;
    int posx;
    int posy;
    int picture_num;
    int speed;
    int speed_mutiple;

    VOID MoveMent_Paint();
    VOID MoveMent_Controller(WPARAM);
    VOID Move_InterAct();
    VOID Attack_Controller();
    VOID Attack_Paint();
};

Character Warrior;

//静态物品
class GOODS
{
public:
    GOODS();
    GOODS(int, int, int, int);
    ~GOODS();
    int size_x, size_y;
    int pos_x, pos_y;
    HBITMAP bitmap;

    VOID TransparentBltPrint(COLORREF);
    VOID BitBltPrint();
};

GOODS Town(Wnd_width / 2, Wnd_height - 200, 200, 100);
GOODS BackHome(Wnd_width - 300, Wnd_height / 2, 200, 100);
GOODS Treasure(Wnd_width - 100, 400, 70, 50);
GOODS Back1(1000, 0, 70, 50);
GOODS Back2(1000, Wnd_height - 100, 70, 50);
GOODS GoldenDragon(Wnd_width / 2, Wnd_height / 2, 100, 100);
GOODS cLoading(Wnd_width / 2 + 500, Wnd_height / 2 - 100, 200, 150);
GOODS Girl1(0, Wnd_height - 500, 400, 500);
GOODS Girl2(Wnd_width - 400, Wnd_height - 500, 400, 500);

class Boss
{
public:
    Boss();
    ~Boss();
    int patience;
    int posx;
    int posy;
    int xsize;
    int ysize;
    int speed;
    BOOL state;
    HBITMAP bitmap;

    VOID Movement_Controller();
    VOID MovePaint();
    VOID Controller();
};

Boss Sword;


// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HDC hdc = NULL, Assist_dc = NULL, bufdc = NULL;

//关卡
INT LeveL;
INT hGame = 1;

//背景位图
HBITMAP Bg_bitmap[10];

//角色位图
HBITMAP Character_bitmap[10];

//雪花
HBITMAP Snow_Bitmap;
Snow SnowFlowers[Snow_Number];
int SnowCount;
RECT SnowRect;//记录雪花的坐标

//时间管理大师
DWORD Pre, Now, Pre1, LoadingPre1;

//事件管理
INT FrameNum;

//是否触发
BOOL Sword_Trigger;

//Retry位图
HBITMAP Retry;

//滚动背景
INT BgOffset;

//Sword的攻击
std::vector<SwordBullets>Sword_Bucket;

//LoadingController
HBITMAP Loading_bitmap[20];
int LoadingCount = 1; 
BOOL Loading = FALSE;
BOOL Is_Loaded = FALSE;

//Ending
BOOL Is_ended;




// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL GameStory();
BOOL Game_Init(HWND);
VOID Game_Main(HWND);
VOID Debug();
HBITMAP LoadImage_FromFile(const wchar_t*, int, int);
VOID Trigger_Send();
VOID MouseMove_Controller(WPARAM, LPARAM);
VOID ButtonDown_Controller(WPARAM, LPARAM);
VOID Remake_Controller();
BOOL Check_InterAct(Character&, GOODS&);
VOID Speed_Print();

VOID Attack_Controller();
VOID AttackPaint();

VOID patience_Print();

VOID Game_CleanUp(HWND);






//窗口主函数
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码.
    while (GameStory() != TRUE);
    LeveL = 1;


    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMEDEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:

    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        0, 0, Wnd_width, Wnd_height, nullptr, nullptr, hInstance, nullptr);

    MoveWindow(hWnd, Wnd_initx, Wnd_inity, Wnd_width, Wnd_height, TRUE);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    if (!Game_Init(hWnd))
    {
        MessageBox(hWnd, L"资源初始化失败", L"消息窗口", 0);
        return FALSE;
    }


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMEDEMO));

    MSG msg = { 0 };

    // 主消息循环:
    while (msg.message!=WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Now = GetTickCount();
            
            if (!Loading)
            {
                if (Now - Pre >= 50)
                {
                    Game_Main(hWnd);
                }
            }
            else
            {
                if (Now - LoadingPre1 >= 100)
                {
                    SelectObject(bufdc, Loading_bitmap[LoadingCount % 11]);
                    BitBlt(Assist_dc, 0, 0, Wnd_width, Wnd_height, bufdc, 0, 0, SRCCOPY);
              
                    cLoading.TransparentBltPrint(RGB(125, 125, 125));
                    LoadingCount++;
                    
                    LoadingPre1 = GetTickCount();
                    BitBlt(hdc, 0, 0, Wnd_width, Wnd_height, Assist_dc, 0, 0, SRCCOPY);

                    if (LoadingCount == 55 && !Is_ended)
                    {
                        LoadingCount = 1;
                        Loading = FALSE;
                    }
                }
            }
        }
    }

    return (int) msg.wParam;
}


//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = 0;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAMEDEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        Game_CleanUp(hWnd);
        PostQuitMessage(0);
        break;

    case WM_MOUSEMOVE:
        MouseMove_Controller(wParam, lParam);
        break;

    case WM_LBUTTONDOWN:
        ButtonDown_Controller(wParam, lParam);
        break;

    case WM_KEYDOWN:
        Warrior.MoveMent_Controller(wParam);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


//游戏初始化,加载资源
BOOL Game_Init(HWND hWnd)
{
    //随机种子
    srand((unsigned)time(NULL));

    //获得dc建立dc
    HBITMAP bmp;
    hdc = GetDC(hWnd);
    Assist_dc = CreateCompatibleDC(hdc);
    bufdc = CreateCompatibleDC(hdc);

    //建立与hdc兼容的空位图对象。
    bmp = CreateCompatibleBitmap(hdc, Wnd_width, Wnd_height);
    SelectObject(Assist_dc, bmp);

    //调试区
    //LeveL = 5;

    //加载位图
    Bg_bitmap[0] = LoadImage_FromFile(L"GameOver.bmp", Wnd_width, Wnd_height);
    Bg_bitmap[1] = LoadImage_FromFile(L"Sai.bmp", Wnd_width, Wnd_height);
    Bg_bitmap[2] = LoadImage_FromFile(L"Girl1.bmp", Wnd_width, Wnd_height);
    Bg_bitmap[3] = LoadImage_FromFile(L"DragonBg.bmp", Wnd_width, Wnd_height);
    Bg_bitmap[4] = LoadImage_FromFile(L"SwordBg.bmp", Wnd_width, Wnd_height);
    Bg_bitmap[5] = LoadImage_FromFile(L"EndingBg.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[0] = LoadImage_FromFile(L"Loading1.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[1] = LoadImage_FromFile(L"Loading2.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[2] = LoadImage_FromFile(L"Loading3.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[3] = LoadImage_FromFile(L"Loading4.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[4] = LoadImage_FromFile(L"Loading5.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[5] = LoadImage_FromFile(L"Loading6.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[6] = LoadImage_FromFile(L"Loading7.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[7] = LoadImage_FromFile(L"Loading8.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[8] = LoadImage_FromFile(L"Loading9.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[9] = LoadImage_FromFile(L"Loading10.bmp", Wnd_width, Wnd_height);
    Loading_bitmap[10] = LoadImage_FromFile(L"Loading11.bmp", Wnd_width, Wnd_height);
    Character_bitmap[0] = LoadImage_FromFile(L"character_0.bmp", 600, 450);
    Snow_Bitmap = LoadImage_FromFile(L"Snow.bmp", 30, 30);
    Retry = LoadImage_FromFile(L"Retry.bmp", 100, 50);
    cLoading.bitmap = LoadImage_FromFile(L"Loading.bmp", cLoading.size_x, cLoading.size_y);




    GetClientRect(hWnd, &SnowRect);//获得用户区大小


    //Game_Main(hWnd);
    return TRUE;
}

//游戏的心脏
VOID Game_Main(HWND hWnd)
{
    if (LeveL == 0)//GameOver层
    {
        //0层的背景图
        SelectObject(bufdc, Bg_bitmap[0]);
        BitBlt(Assist_dc, 0, 0, Wnd_width, Wnd_height, bufdc, 0, 0, SRCCOPY);

        //Character_0贴再Assist_dc中
        SelectObject(bufdc, Character_bitmap[0]);
        BitBlt(Assist_dc, Wnd_width-300, Wnd_height-450, 300, 450, bufdc, 300, 0, SRCAND);
        BitBlt(Assist_dc, Wnd_width-300, Wnd_height-450, 300, 450, bufdc, 0, 0, SRCPAINT);


        //雪花贴图
        if (SnowCount < Snow_Number) {
            SnowFlowers[SnowCount].pos_x = rand() % SnowRect.right;
            SnowFlowers[SnowCount].pos_y = 0;
            SnowFlowers[SnowCount].exist = TRUE;
            SnowCount++;
        }

        //遍历雪花看是否存在
        for (int i = 0; i < Snow_Number; i++)
        {
            Snow& Cur = SnowFlowers[i];
            if (Cur.exist)
            {
                //贴图
                SelectObject(bufdc, Snow_Bitmap);
                TransparentBlt(Assist_dc, Cur.pos_x, Cur.pos_y, Cur.size_x, Cur.size_y, bufdc, 0, 0, Cur.size_x, Cur.size_y, RGB(0, 0, 0));

                //随机决定左右移动方向
                if (rand() % 2)Cur.pos_x += rand() % 6;
                else Cur.pos_x -= rand() % 6;

                //飘落
                Cur.pos_y += 10;

                //判断是否超出界面
                if (Cur.pos_x<0 || Cur.size_x>SnowRect.right)Cur.exist = FALSE;
                if (Cur.pos_y > SnowRect.bottom)Cur.pos_x = rand() % SnowRect.right, Cur.pos_y = 0;
            }
        }


        //Retry
        SelectObject(bufdc, Retry);
        TransparentBlt(Assist_dc, Wnd_width / 2 - 50, Wnd_height / 2 - 25, 100, 50, bufdc, 0, 0, 100, 50, RGB(0, 0, 0));


    }
    else if (LeveL == 1)//最初层
    {
        if (!Is_Loaded)
        {
            Loading = TRUE;
            Is_Loaded = TRUE;
        }
        SelectObject(bufdc, Bg_bitmap[1]);
        BitBlt(Assist_dc, 0, 0, Wnd_width, Wnd_height, bufdc, 0, 0, SRCCOPY);

        Warrior.MoveMent_Paint();

        
        Town.bitmap = LoadImage_FromFile(L"Town.bmp", Town.size_x, Town.size_y);
        Town.TransparentBltPrint(RGB(125, 125, 125));

        BackHome.bitmap = LoadImage_FromFile(L"BackHome.bmp", BackHome.size_x, BackHome.size_y);
        BackHome.TransparentBltPrint(RGB(125, 125, 125));

        Speed_Print();
        
    }
    else if (LeveL == 2)
    {
       
        if (!Is_Loaded)
        {
            Loading = TRUE;
            Is_Loaded = TRUE;
        }

        SelectObject(bufdc, Bg_bitmap[2]);
        BitBlt(Assist_dc, 0, 0, Wnd_width, Wnd_height, bufdc, 0, 0, SRCCOPY);

        Warrior.MoveMent_Paint();

        Treasure.bitmap = LoadImage_FromFile(L"Treasures.bmp", Treasure.size_x, Treasure.size_y);
        Treasure.TransparentBltPrint(RGB(255, 255, 255));

        Speed_Print();

        Back1.bitmap = LoadImage_FromFile(L"Back.bmp", Back1.size_x, Back1.size_y);
        Back1.TransparentBltPrint(RGB(125, 125, 125));

        GoldenDragon.bitmap = LoadImage_FromFile(L"GoldenDragon.bmp", GoldenDragon.size_x, GoldenDragon.size_y);
        GoldenDragon.TransparentBltPrint(RGB(0, 0, 0));

    }
    else if (LeveL == 3)
    {
        
        if (!Is_Loaded)
        {
            Loading = TRUE;
            Is_Loaded = TRUE;
        }

        SelectObject(bufdc, Bg_bitmap[3]);
        BitBlt(Assist_dc, 0, 0, Wnd_width, Wnd_height, bufdc, 0, 0, SRCCOPY);
        
        Warrior.MoveMent_Paint();

        Back2.bitmap = LoadImage_FromFile(L"Back.bmp", Back2.size_x, Back2.size_y);
        Back2.TransparentBltPrint(RGB(125, 125, 125));
        
        GoldenDragon.bitmap = LoadImage_FromFile(L"GoldenDragon.bmp", GoldenDragon.size_x, GoldenDragon.size_y);
        GoldenDragon.TransparentBltPrint(RGB(0, 0, 0));

        Speed_Print();
    }
    else if (LeveL == 4 && hGame == 1)
    {
       
        if (!Is_Loaded)
        {
            Loading = TRUE;
            Is_Loaded = TRUE;
        }

        SelectObject(bufdc, Bg_bitmap[4]);
        BitBlt(Assist_dc, BgOffset, 0, Wnd_width - BgOffset, Wnd_height, bufdc, 0, 0, SRCCOPY);
        BitBlt(Assist_dc, 0, 0, BgOffset, Wnd_height, bufdc, Wnd_width - BgOffset, 0, SRCCOPY);
        
        BgOffset += 20;
        BgOffset %= Wnd_width;


        if (!Sword_Trigger)
        {
            MessageBox(NULL, L"我们的行踪被Sword所察觉,一场大战在所难免。\n 你先别慌,Sword最讨厌的就是怯战小子,如果你不和他交手他就会失去耐心,也许这样我们能逃过一劫！\n证明自己不是一个菜鸡好吗!", L"Notice!", MB_OK);
            Sword_Trigger = TRUE;
        }

        Sword.Controller();


        AttackPaint();


        Sword.Movement_Controller();
        Sword.MovePaint();
        
        Warrior.MoveMent_Paint();


        Speed_Print();
        patience_Print();
        
    }
    else if (LeveL == 4 && hGame == 2)
    {
        
        SelectObject(bufdc, Bg_bitmap[5]);
        BitBlt(Assist_dc, 0, 0, Wnd_width, Wnd_height, bufdc, 0, 0, SRCCOPY);

        Warrior.MoveMent_Paint();

        GoldenDragon.bitmap = LoadImage_FromFile(L"GoldenDragon.bmp", GoldenDragon.size_x, GoldenDragon.size_y);
        GoldenDragon.pos_x = Wnd_width / 2 - 40;
        GoldenDragon.pos_y = Wnd_height / 2 - 150;
        GoldenDragon.TransparentBltPrint(RGB(12, 12, 12));


    }
    else if (LeveL == 5)
    {
        
        SelectObject(bufdc, Bg_bitmap[0]);
        BitBlt(Assist_dc, 0, 0, Wnd_width, Wnd_height, bufdc, 0, 0, SRCCOPY);

        //Character_0贴再Assist_dc中
        Girl1.bitmap = LoadImage_FromFile(L"character_1.bmp", Girl1.size_x, Girl1.size_y);
        Girl2.bitmap = LoadImage_FromFile(L"character_2.bmp", Girl2.size_x, Girl2.size_y);
        Girl1.TransparentBltPrint(RGB(0, 0, 0));
        Girl2.TransparentBltPrint(RGB(0, 0, 0));


        //雪花贴图
        if (SnowCount < Snow_Number) {
            SnowFlowers[SnowCount].pos_x = rand() % SnowRect.right;
            SnowFlowers[SnowCount].pos_y = 0;
            SnowFlowers[SnowCount].exist = TRUE;
            SnowCount++;
        }

        //遍历雪花看是否存在
        for (int i = 0; i < Snow_Number; i++)
        {
            Snow& Cur = SnowFlowers[i];
            if (Cur.exist)
            {
                //贴图
                SelectObject(bufdc, Snow_Bitmap);
                TransparentBlt(Assist_dc, Cur.pos_x, Cur.pos_y, Cur.size_x, Cur.size_y, bufdc, 0, 0, Cur.size_x, Cur.size_y, RGB(0, 0, 0));

                //随机决定左右移动方向
                if (rand() % 2)Cur.pos_x += rand() % 6;
                else Cur.pos_x -= rand() % 6;

                //飘落
                Cur.pos_y += 10;

                //判断是否超出界面
                if (Cur.pos_x<0 || Cur.size_x>SnowRect.right)Cur.exist = FALSE;
                if (Cur.pos_y > SnowRect.bottom)Cur.pos_x = rand() % SnowRect.right, Cur.pos_y = 0;
            }
        }

        //Ending
        SelectObject(bufdc, Retry);
        TransparentBlt(Assist_dc, Wnd_width / 2 - 50, Wnd_height / 2 - 25, 100, 50, bufdc, 0, 0, 100, 50, RGB(0, 0, 0));
    }


    BitBlt(hdc, 0, 0, Wnd_width, Wnd_height, Assist_dc, 0, 0, SRCCOPY);//将Assist_dc中的内容全部贴再hdc上
    
    //触发函数
    Trigger_Send();

    Pre = GetTickCount();
}

//背景故事
BOOL GameStory()
{
    wchar_t Story[] = L"从前有一个勇士他要打败恶魔黄金龙营救公主，恶魔黄金龙十分不好对付，何况他的手下Sword称得上是世界上数一数二的剑士.\n但是我们的勇者丝毫不惧，让我们协助他讨伐恶龙吧！";
    if (MessageBox(NULL, Story, L"BgStory", MB_OKCANCEL) == IDOK)
    {
        if (MessageBox(NULL, L"你愿意加入吗?", L"Make Your Choice!", MB_OKCANCEL) == IDOK)return TRUE;
    }
    return FALSE;
}

//Debug
VOID Debug()
{
    MessageBox(NULL, L"no more!!", L"Notice!", 0);
}

//便利函数
HBITMAP LoadImage_FromFile(const wchar_t* filename, int size_x, int size_y)
{
    return (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, size_x, size_y, LR_LOADFROMFILE);
}

//触发函数
VOID Trigger_Send()
{
    //不知道为什么的问题
    
}

//鼠标悬停互动
VOID MouseMove_Controller(WPARAM wParam, LPARAM lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    if (LeveL == 0 || LeveL == 5)
    {
        if (x >= Wnd_width / 2 - 50 && x <= Wnd_width / 2 + 50 && y >= Wnd_height / 2 - 25 && y <= Wnd_height / 2 + 25)
        {
            HCURSOR cursor = LoadCursor(NULL, IDC_HAND);
            SetCursor(cursor);
        }
    }
}

//鼠标点击互动
VOID ButtonDown_Controller(WPARAM wParam, LPARAM lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    if (LeveL == 0)
    {
        if (x >= Wnd_width / 2 - 50 && x <= Wnd_width / 2 + 50 && y >= Wnd_height / 2 - 25 && y <= Wnd_height / 2 + 25)
        {
            PlaySound(L"BGM1.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //循环播放背景音乐
            LeveL = 1;
            Loading = FALSE;
        }
    }
    else if (LeveL == 5)
    {
        if (x >= Wnd_width / 2 - 50 && x <= Wnd_width / 2 + 50 && y >= Wnd_height / 2 - 25 && y <= Wnd_height / 2 + 25)
        {
            PlaySound(L"鸟之诗.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //循环播放背景音乐 
            if (MessageBox(NULL, L"游戏已经结束了！勇士们踏上属于你们的征途吧！", L"Ending!", MB_OK)==IDOK)
            {
                MessageBox(NULL, L"感谢QianMo先生的《逐梦旅程：Windows游戏编程之从零开始》中的素材和对GDI使用的部分对作者的启发，在本人怀揣的游戏开发梦想却无力实现时给了作者希望和方向，希望QianMo先生在天堂可以看到中国3A走向世界的那一天！再次感谢QianMo先生！", L"Thanks!", MB_OK);
            }
            Loading = TRUE;
            Is_ended = TRUE;
        }
    }
}

//人物构造函数
Character::Character()
{
    xsize = 40;
    ysize = 90;
    posx = 50;
    posy = 50;
    direction = 3;
    speed = 10;
    speed_mutiple = 1;
    bitmap[0] = LoadImage_FromFile(L"go1.bmp", xsize*8, ysize*2);
    bitmap[1] = LoadImage_FromFile(L"go2.bmp", xsize*8, ysize*2);
    bitmap[2] = LoadImage_FromFile(L"go3.bmp", xsize*8, ysize*2);
    bitmap[3] = LoadImage_FromFile(L"go4.bmp", xsize*8, ysize*2);
}

Character::~Character()
{
    for (int i = 0; i < 4; i++)DeleteObject(bitmap[i]);
}

//人物移动贴图
VOID Character::MoveMent_Paint()
{
    SelectObject(bufdc, bitmap[direction]);
    BitBlt(Assist_dc, posx, posy, xsize, ysize, bufdc, picture_num * xsize, ysize, SRCAND);
    BitBlt(Assist_dc, posx, posy, xsize, ysize, bufdc, picture_num * xsize, 0, SRCPAINT);

    picture_num++;
    picture_num %= 8;
}

//人物移动互动管理
VOID Character::Move_InterAct()
{
    if (LeveL == 1)
    {
        if (Check_InterAct(Warrior,Town))
        {
            if (MessageBox(NULL, L"要进入小镇吗?", L"Confirm!", MB_OKCANCEL) == IDOK)
            {
                PlaySound(L"BGM1.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //循环播放背景音乐 
                LeveL++;
                Warrior.posx = 50;
                Warrior.posy = 50;
            }
        }
        if (Check_InterAct(Warrior, BackHome))
        {
            if (MessageBox(NULL, L"你Der啊!确定要放弃公主吗?", L"Confirm!", MB_OKCANCEL) == IDOK)
            {
                PlaySound(L"BGM2.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //循环播放背景音乐
                LeveL--;
                Warrior.posx = 50;
                Warrior.posy = 50;
                //重置勇者的属性
                Warrior.speed_mutiple = 1;
            }
        }
    }
    else if (LeveL == 2)
    {
        if (Check_InterAct(Warrior, Treasure))
        {
            if (Warrior.speed_mutiple < 3)
            {
                Warrior.speed_mutiple++;
                MessageBox(NULL, L"勇者获得了某种不知名的力量，这下牛逼了\n勇者感觉充满了力量！", L"Notice!", MB_OK);
            }
            else 
            {
                MessageBox(NULL, L"勇者似乎习惯了这种力量。", L"Notice!", MB_OK);
            }
        }
        else if (Check_InterAct(Warrior, Back1))
        {
            if (MessageBox(NULL, L"要回到上一个界面吗？", L"Notice!", MB_OKCANCEL) == IDOK)
            {
                LeveL--;
                Warrior.posx = 50;
                Warrior.posy = 50;
            }
        }
        else if (Check_InterAct(Warrior, GoldenDragon))
        {
            if (MessageBox(NULL, L"准备好挑战恶龙了吗", L"Confirm!", MB_OKCANCEL) == IDOK)
            {
                
                LeveL++;
                Warrior.posx = 50;
                Warrior.posy = 50;
            }
        }
    }
    else if (LeveL == 3)
    {
        if (Check_InterAct(Warrior, GoldenDragon))
        {
            LeveL++;
            Warrior.posx = 50;
            Warrior.posy = 50;
        }
        if (Check_InterAct(Warrior, Back2))
        {
            if (MessageBox(NULL, L"要回到小镇吗？", L"Notice!", MB_OKCANCEL) == IDOK)
            {
                LeveL--;
                Warrior.posx = 50;
                Warrior.posy = 50;
            }
        }
    }
    else if (LeveL == 4 && hGame == 1)
    {
        for (auto& item : Sword_Bucket)
        {
            if (item.exist)
            {
                if (item.posx <= posx && item.posy >= posy && item.posx + 200 >= posx + xsize && item.posy + item.ysize <= posy + ysize)
                {
                    MessageBox(NULL, L"佛悟啊哥们,勇者的身体着穿透了,赶快修炼一下吧!", L"Notice", MB_OK);
                    LeveL = 0;
                    Remake_Controller();
                    break;
                }
            }
        }
    }
    else if (LeveL == 4 && hGame == 2)
    {
        if (Check_InterAct(Warrior, GoldenDragon))
        {
            MessageBox(NULL, L"游戏到这里就戛然而止了,感谢你的游玩,我的人生就像这款游戏一样,不断地半途而废,也许真的藏着一条所谓的GoldenDragon,也许真的有公主等待着营救,但是在我的故事中,一切已经结束了,这一次没有重来了。每一次半途而废都是为下一次半途而废做铺垫,希望还能有下一个游戏吧。", L"Ending", MB_OK);
            LeveL++;

        }
    }
}

//人物移动消息函数
VOID Character::MoveMent_Controller(WPARAM wParam)
{
    if (wParam == VK_UP)
    {
        direction = 0;
        posy -= speed * speed_mutiple;
        if (posy < 0)posy = 0;
    }
    else if (wParam == VK_DOWN)
    {
        direction = 1;
        posy += speed * speed_mutiple;
        if (posy > Wnd_height - ysize * 2)posy = Wnd_height - ysize * 2;
    }
    else if (wParam == VK_LEFT)
    {
        direction = 2;
        posx -= speed * speed_mutiple;
        if (posx < 0)posx = 0;
    }
    else if (wParam == VK_RIGHT)
    {
        direction = 3;
        posx += speed * speed_mutiple;
        if (posx > Wnd_width - xsize * 2)posx = Wnd_width - xsize * 2;
    }

    Move_InterAct();
}

//还没有此功能
VOID Character::Attack_Controller()
{

}

//还没有此功能
VOID Character::Attack_Paint()
{

}

//判断是否执行互动
BOOL Check_InterAct(Character& C,GOODS& goods)
{
    if (C.posx + C.xsize >= goods.pos_x && C.posx <= goods.pos_x + goods.size_x && C.posy + C.ysize >= goods.pos_y && C.posy <= goods.pos_y + goods.size_y)
    {
        if (C.posx + C.xsize >= goods.pos_x)C.posx = goods.pos_x - C.xsize;
        else if (C.posx <= goods.pos_x + goods.size_x)C.posx = goods.pos_x + goods.size_x;
        else if (C.posy + C.ysize >= goods.pos_y) C.posy = goods.pos_y - C.ysize;
        else if (C.posy <= goods.pos_y + goods.size_y)C.posy = goods.pos_y + goods.size_y;
        return TRUE;
    }
    return FALSE;
}

//物品构造函数
GOODS::GOODS()
{

}

GOODS::GOODS(int px,int py,int sx,int sy)
{
    size_x = sx;
    size_y = sy;
    pos_x = px;
    pos_y = py;
}

GOODS::~GOODS()
{
    DeleteObject(bitmap);
}

//物品的透明色彩法
VOID GOODS::TransparentBltPrint(COLORREF color)
{
    SelectObject(bufdc, bitmap);
    TransparentBlt(Assist_dc, pos_x, pos_y, size_x, size_y, bufdc, 0, 0, size_x, size_y,color);
}

//物品的透明遮罩法
VOID GOODS::BitBltPrint()
{
    SelectObject(bufdc, bitmap);
    BitBlt(Assist_dc, pos_x, pos_y, size_x, size_y, bufdc, size_x, 0, SRCAND);
    BitBlt(Assist_dc, pos_x, pos_y, size_x, size_y, bufdc, 0, 0, SRCPAINT);
}

//Boss的构造函数
Boss::Boss()
{
    patience = 100;
    speed = 20;
    posx = Wnd_width - 300;
    xsize = 200;
    posy = Wnd_height - 200;
    ysize = 150;
    state = FALSE;
    bitmap = LoadImage_FromFile(L"swordman.bmp", xsize, ysize);
}

Boss::~Boss()
{
    DeleteObject(bitmap);
}

//Boss移动状态管理
VOID Boss::Movement_Controller()
{
    if (!state)
    {
        if (Warrior.posy > posy)posy += speed;
        else posy -= speed;
    }
    else
    {
        if (rand() % 2)posy += speed;
        else posy -= speed;
    }
}

//Boss的移动贴图
VOID Boss::MovePaint()
{
    SelectObject(bufdc, bitmap);
    TransparentBlt(Assist_dc, posx, posy, xsize, ysize, bufdc, 0, 0, xsize, ysize, RGB(0, 0, 0));
}

//Boss的攻击和状态管理
VOID Boss::Controller()
{
    if (Now - Pre1 >= 500 && hGame == 1)
    {
        Attack_Controller();
        Pre1 = GetTickCount();
    }
    if (Sword.patience <= 50)
    {
        Sword.state = TRUE;

    }
    if (Sword.patience == 0)
    {
        MessageBox(NULL, L"你成功躲过了Sword的追杀，牛逼了我的哥!", L"Notice", MB_OK);
        hGame = 2;
        Warrior.posx = 50;
        Warrior.posy = 50;
    }
}

//Boss的攻击方式管理 待完善
VOID Attack_Controller()
{
    if (Sword.state == FALSE)
    {
        SwordBullets cur;
        Sword.patience--;
        Sword_Bucket.push_back(cur);
        for (auto& item : Sword_Bucket)
        {
            item.posx -= 150;
            if (item.posx < 0)item.exist = FALSE;
        }
    }
    else
    {
        SwordBullets cur;
        Sword.patience--;
        Sword_Bucket.push_back(cur);
        for (auto& item : Sword_Bucket)
        {
            item.posx -= 100;
            if (item.posx < 0)item.exist = FALSE;
        }
    }
}

//Boss的飞刀贴图
VOID AttackPaint()
{
    for (auto& item : Sword_Bucket)
    {
        if (item.exist)
        {
            SelectObject(bufdc, LoadImage_FromFile(L"swordblade.bmp", item.xsize, item.ysize));
            TransparentBlt(Assist_dc, item.posx, item.posy, item.xsize, item.ysize, bufdc, 0, 0, item.xsize, item.ysize, RGB(0, 0, 0));
        }
    }
}

//Boss飞刀的构造函数
SwordBullets::SwordBullets()
{
    posx = Sword.posx - 50;
    posy = Sword.posy + Sword.ysize / 2;
    xsize = 50;
    ysize = 25;
    exist = TRUE;
    bitmap = LoadImage_FromFile(L"swordblade.bmp", xsize, ysize);
}

SwordBullets::~SwordBullets()
{
    DeleteObject(bitmap);
}

//重置函数
VOID Remake_Controller()
{
    Warrior.posx = 50;
    Warrior.posy = 50;
    Sword_Trigger = FALSE;
    Warrior.speed_mutiple = 1;
    Sword.patience = 100;
    Sword.posy = Wnd_height - 200;
    Sword_Bucket.clear();
    Is_Loaded = FALSE;
}

//贴出人物的移速
VOID Speed_Print()
{
    wchar_t str[20];
    HFONT hfont=CreateFont(20, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
    SelectObject(Assist_dc, hfont);
    SetBkMode(Assist_dc, TRANSPARENT);
    SetTextColor(Assist_dc, RGB(0, 125, 125));
    swprintf_s(str, L"勇者的速度为:%d", Warrior.speed * Warrior.speed_mutiple);
    TextOut(Assist_dc, 0, 0, str, wcslen(str));
}

//贴出Boss的耐心
VOID patience_Print()
{
    wchar_t str[20];
    HFONT hfont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
    SelectObject(Assist_dc, hfont);
    SetBkMode(Assist_dc, TRANSPARENT);
    SetTextColor(Assist_dc, RGB(255, 125, 0));
    swprintf_s(str, L"Sword的耐心值:%d", Sword.patience);
    TextOut(Assist_dc, Wnd_width / 2, 0, str, wcslen(str));
}

//定义清理函数
VOID Game_CleanUp(HWND hWnd)
{
    ReleaseDC(hWnd, hdc);
    DeleteDC(Assist_dc);
    DeleteDC(bufdc);
    for (int i = 0; i < 5; i++)
    {
        DeleteObject(Bg_bitmap[i]);
    }
    for (int i = 0; i < 12; i++)
    {
        DeleteObject(Loading_bitmap[i]);
    }
    DeleteObject(Retry);
}

