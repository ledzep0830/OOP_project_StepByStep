/*
NormalAccount 클래스는      '보통예금계좌'를 의미
                            객체의 생성 과정에서 이율 정보를 등록할 수 있도록 정의
HighCreditAccount 클래스는  '신용신뢰계좌'를 의미
                            다음의 특성을 부여하여 정의
                                1. NormalAccount 클래스처럼 객체 생성 과정에서 기본 이율을 등록할 수 있음
                                2. 고객 신용등급을 A, B, C로 나누고 계좌 개설 시 이 정보를 등록함
                                3. A, B, C 등급별로 기본 이율에 각각 7%, 4%, 2% 추가 이율을 제공함

이자는 시간이 지나야 발생하지만, 여기서는 구현의 편의상 입금 시 이자가 원금에 더해지는 것으로 간주함
모든 계좌에 대해 다음의 조건을 적용
    1. 계좌 개설 과정에서 초기 입금 금액에 대해서는 이자를 계산하지 않음
    2. 계좌 개설 후 별도의 입금 과정을 거칠 때에만 이자가 발생하는 것으로 간주함
    3. 이자의 계산 과정에서 발생하는 소수점 이하의 금액은 무시함

또한 컨트롤 클래스인 AccountHandler 클래스에는 큰 변화가 없어야 함
*/

#define _CRT_SECURE_NO_WARNINGS // SDL 관련 경고를 제거
#include <cstdio>
#include <cstring>
using namespace std;

// 신용 등급
enum  {
    RankA = 7,
    RankB = 4,
    RankC = 2
};

// Entity 클래스
class BankAccount {
    private:
        int AccountNumber; // 계좌번호
        char* Name;        // 고객 이름
        int Money;         // 고객 잔액

    public:
        BankAccount(int num, char* name, int money);    // 생성자
        BankAccount(const BankAccount &copy);           // 추가된 복사 생성자
        int GetNumber() const; 
        virtual void Deposit(int money);                // Entity 클래스에 virtual 추가하는 것이 중요
        int Withdraw(int money);
        void ShowAccountInfo() const;
        ~BankAccount();                                 // 소멸자
};

// 생성자
BankAccount::BankAccount(int num, char* name, int money):AccountNumber(num),Money(money) {
    Name = new char[strlen(name) + 1];
    strcpy(Name, name);
}

// 추가된 복사 생성자
BankAccount::BankAccount(const BankAccount &copy):AccountNumber(copy.AccountNumber),Money(copy.Money) {
    Name = new char[strlen(copy.Name)];
    strcpy(Name, copy.Name);
}

int BankAccount::GetNumber() const {
    return AccountNumber;
} 

void BankAccount::Deposit(int money) {
    Money += money;
}

int BankAccount::Withdraw(int money) {
    if(Money < money)
        return 0;
    
    Money -= money;
    return money;
}

void BankAccount::ShowAccountInfo() const {
    printf("이름: %s\n", Name);
    printf("계좌번호: %d\n", AccountNumber);
    printf("잔고: %d\n", Money);
}

// 소멸자
BankAccount::~BankAccount() {
    delete[]Name;
}

class NormalAccount:public BankAccount {
    private:
        int ratio;
    public:
        NormalAccount(int num, char* name, int money, int profits) :BankAccount(num, name, money), ratio(profits) {}
        virtual void Deposit(int money) {
            BankAccount::Deposit(money);                    // 원금
            BankAccount::Deposit(money*(ratio/100.0));      // 이자
        }
};

class HighCreditAccount :public NormalAccount {
    private:
        int credit;
    public:
        HighCreditAccount(int num, char* name, int money, int profits, int credibility) :NormalAccount(num, name, money, profits), credit(credibility) {}
        virtual void Deposit(int money) {
            NormalAccount::Deposit(money);                  // 원금 이자
            BankAccount::Deposit(money*(credit/100.0));     // 추가 이자
        }
};

// 컨트롤 클래스
class AccountHandler {
    private:
        BankAccount* Arr[10];           // 계좌 포인터 배열
        int AccountNum;                 // 계좌 개수
    public:
        AccountHandler();
        ~AccountHandler();
        int Menu(void);                 // 메뉴
        void MakeBankAccount(void);     // 계좌 개설
        void Deposit(int money);        // 입금
        void Withdraw(int money);       // 출금
        void ShowAccountInfo(void);     // 전체 고객 잔액 조회
    private:                            // protected는 외부에서 보면 private, 상속관계에서 보면 public
        void MakeNormalAccount(void);
        void MakeCreditAccount(void);
};

AccountHandler::AccountHandler() :AccountNum(0) {}

AccountHandler::~AccountHandler() {
    for(int i=0; i<AccountNum; i++)
        delete Arr[i];
}

int AccountHandler::Menu(void)  {
    int sel;
    printf("-----Menu-----\n");
    printf("1. 계좌 개설 \n");
    printf("2. 입금 \n");
    printf("3. 출금 \n");
    printf("4. 계좌 정보 전체 출력 \n");
    printf("5. 프로그램 종료 \n");
    printf("선택: ");
    scanf("%d", &sel);
    return sel;
}

void AccountHandler::MakeBankAccount() {
    int sel;
    printf("계좌 종류를 선택하시오 \n");
    printf("1. 보통 계좌\t2. 신용 계좌 \n");
    printf("선택: ");
    scanf("%d", &sel);

    if(sel==1) 
        MakeNormalAccount();
    else if(sel==2)
        MakeCreditAccount();
    else {
        while(sel!=1 && sel!=2) {
            printf("번호를 잘못 선택하셨습니다.. 다시! \n");
            printf("선택: ");
            scanf("%d", &sel);
            if(sel==1)
                MakeNormalAccount();
            else if(sel==2)
                MakeCreditAccount();
        }
    }
}

void AccountHandler::MakeNormalAccount(void) {
    int accountNumber;
    char name[30];
    int money;
    int ratio;

    printf("%d번째 사람 정보 입력 \n", AccountNum+1);
    printf("계좌 번호 입력: ");
    scanf("%d", &accountNumber);
    printf("이름 입력: ");
    scanf("%s", name);
    printf("잔액 입력: ");
    scanf("%d", &money);
    printf("이율 입력: ");
    scanf("%d", &ratio);

    Arr[AccountNum++] = new NormalAccount(accountNumber, name, money, ratio);
}

void AccountHandler::MakeCreditAccount(void) {
    int accountNumber;
    char name[30];
    int money;
    int ratio;
    int rank;

    printf("%d번째 사람 정보 입력 \n", AccountNum+1);
    printf("계좌 번호 입력: ");
    scanf("%d", &accountNumber);
    printf("이름 입력: ");
    scanf("%s", name);
    printf("잔액 입력: ");
    scanf("%d", &money);
    printf("이율 입력: ");
    scanf("%d", &ratio);
    printf("신용 등급 (1. Rank A, 2. Rank B, 3. Rank C) 입력: ");
    scanf("%d", &rank);

    switch(rank) {
        case 1:
            Arr[AccountNum++] = new HighCreditAccount(accountNumber, name, money, ratio, RankA);
            break;
        case 2:
            Arr[AccountNum++] = new HighCreditAccount(accountNumber, name, money, ratio, RankB);
            break;
        case 3:
            Arr[AccountNum++] = new HighCreditAccount(accountNumber, name, money, ratio, RankC);
            break;
    }
}

void AccountHandler::Deposit(int money) {
    int number;
    printf("고객님의 계좌 번호를 입력해주세요: ");
    scanf("%d", &number);
    for(int i=0; i<AccountNum; i++) {
        if(Arr[i]->GetNumber()==number) {  // 계좌 번호가 같으면
            Arr[i]->Deposit(money);
            printf("입금이 완료되었습니다 \n");
            return;
        }
    }
    printf("없는 계좌 번호입니다 \n");
}

void AccountHandler::Withdraw(int money) {
    int number;
    printf("고객님의 계좌 번호를 입력해주세요: ");
    scanf("%d", &number);
    for(int i=0; i<AccountNum; i++) {
        if(Arr[i]->GetNumber()==number) {   // 계좌 번호가 같으면
            if(Arr[i]->Withdraw(money)==0) {
                printf("잔액이 부족합니다 \n");
                return;
            }
            printf("출금이 완료되었습니다 \n");
            return;
        }
    }
    printf("없는 계좌 번호입니다 \n");
}

void AccountHandler::ShowAccountInfo(void) {
    for(int i=0; i<AccountNum; i++) {
        Arr[i]->ShowAccountInfo();
        printf("\n");
    }
}

int main(void) {
    AccountHandler handler;
    int money;
    
    while(1) {
        int sel = handler.Menu();
        switch(sel) {
            case 1:
                handler.MakeBankAccount();
                break;
            case 2:
                printf("입금할 금액: ");
                scanf("%d", &money);
                handler.Deposit(money);
                break;
            case 3:
                printf("출금할 금액: ");
                scanf("%d", &money);
                handler.Withdraw(money);
                break;
            case 4:
                handler.ShowAccountInfo();
                break;
            case 5:
                return 0;
            default:
                printf("잘못된 번호를 입력하셨습니다 \n");
        }
    }
    return 0;
}
