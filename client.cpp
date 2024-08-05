#include <myhead.h>
#include <iostream>
#include <string>
using namespace std;
enum{
    LOGIN,
    FIND,
    ADD,
    DELETE,
    MODIFY,
    QUIT,
    FIND_NAME
};
typedef struct STAFF{
    int type;
    char passwd[32];
    char name_one[32];
    char name_two[32];
    char age[4];
    char tel[16];
    char major[32];
    char salar[16];
    char home[64];
}sta_t;
class person{
public:
    virtual void person_menu()=0;
    sta_t staff;
};
class staff1:public person{
public:
    void person_menu()override{
        cout <<"---------------------------------" <<endl;
        cout <<"  3.修改信息  4.查看信息  5.退出  " <<endl;
        cout <<"---------------------------------" <<endl;
        cout <<"请输入你的选择>>" <<endl;
    }
};
class root:public person{
public:
    void person_menu()override{
        cout <<"----------------------------------------------------------------" <<endl;
        cout <<"  1.添加员工  2.删除员工  3.修改员工信息  4.查看员工信息  5.退出  " <<endl;
        cout <<"----------------------------------------------------------------" <<endl;
        cout <<"请输入你的选择>>" <<endl;
    }
};
class person_factory{
public:
    static person *create_person(string name){
        if(name=="root"){
            return new root;
        }else if(name=="staff"){
            return new staff1;
        }
        return NULL;
    }
};
void add_staff(int sock_fd,sta_t staff){
    if((strncmp(staff.name_one,"root",4))==0){
        staff.type=ADD;
        cout <<"请依次输入员工信息(空格隔开)" <<endl;
        cout <<"<姓名 密码 年龄 电话 职业 工资 家庭住址>" <<endl;
        cin >>staff.name_two >>staff.passwd >>staff.age >>staff.tel >>staff.major >>staff.salar >>staff.home;
        // scanf("%s %s %s %s %s %s %s",staff.name_two,staff.passwd,staff.age,staff.tel,staff.major,staff.salar,staff.home);
        cout <<"密码一经创建不可修改,只能通过添加删除重新填写信息,请务必让员工记住密码!!!" <<endl;
        // while(getchar()!='\n');
        if((send(sock_fd,&staff,sizeof(sta_t),0))==-1)
            ERR_LOG("send error");
        if((recv(sock_fd,&staff,sizeof(sta_t),0))==-1)
            ERR_LOG("recv error");
        cout <<"添加结果: "<<staff.passwd <<endl;
    }else{
        cout <<"你没有添加员工的权限!!!" <<endl;
    }
}
void delete_staff(int sock_fd,sta_t staff){
    if((strncmp(staff.name_one,"root",4))==0){
        staff.type=DELETE;
        cout <<"请输入需要删除的用户名>>" <<endl;
        cin >>staff.name_two;
        // scanf("%s",staff.name_two);
        // while(getchar()!='\n');
        if((send(sock_fd,&staff,sizeof(sta_t),0))==-1)
            ERR_LOG("send error");
        if((recv(sock_fd,&staff,sizeof(sta_t),0))==-1)
            ERR_LOG("recv error");
        cout <<"删除结果: "<<staff.passwd <<endl;
    }else{
        cout <<"你没有删除员工的权限!!!" <<endl;
    }
}
void modify_staff(int sock_fd,sta_t staff){
    if((strncmp(staff.name_one,"root",4))==0){
        staff.type=FIND_NAME;
        if((send(sock_fd,&staff,sizeof(staff),0))==-1)
            ERR_LOG("send error");
        cout <<"目前已经加入的员工如下:" <<endl;
        while(1){
            if((recv(sock_fd,&staff,sizeof(staff),0))==-1)
                ERR_LOG("recv error");
            if(strcmp(staff.passwd,"查询完毕")==0){
                break;
            }
            cout <<"姓名: " <<staff.name_two <<endl;
        }
        cout <<"请输入你要修改的名字>>" <<endl;
        scanf("%s",staff.name_two);
        while(getchar()!='\n');
    }else{
        strcpy(staff.name_two,staff.name_one);
    }
    if((strncmp(staff.name_one,"root",4))==0){
        cout <<"请依次输入修改的员工信息(空格隔开)" <<endl;
        cout <<"<年龄 电话 职业 工资 家庭住址>" <<endl;
        scanf("%s %s %s %s %s",staff.age,staff.tel,staff.major,staff.salar,staff.home);
        while(getchar()!='\n');
    }else{
        cout <<"请依次输入修改的信息(空格隔开)" <<endl;
        cout <<"若需要修改其他信息,请告知root用户" <<endl;
        cout <<"<年龄 电话 家庭住址>" <<endl;
        scanf("%s %s %s",staff.age,staff.tel,staff.home);
        while(getchar()!='\n');
    }
    staff.type=MODIFY;
    if((send(sock_fd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
    if((recv(sock_fd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("recv error");
    cout <<"修改结果: " <<staff.passwd <<endl;
}
void find_staff(int sock_fd,sta_t staff){
    if((strncmp(staff.name_one,"root",4))==0){
        staff.type=FIND_NAME;
        if((send(sock_fd,&staff,sizeof(staff),0))==-1)
            ERR_LOG("send error");
        cout <<"目前已经加入的员工如下:" <<endl;
        while(1){
            if((recv(sock_fd,&staff,sizeof(staff),0))==-1)
                ERR_LOG("recv error");
            if(strcmp(staff.passwd,"查询完毕")==0){
                break;
            }
            cout <<"姓名: "<<staff.name_two <<endl;
        }
        cout <<"请输入你要查询的名字>>" <<endl;
        scanf("%s",staff.name_two);
        while(getchar()!='\n');
    }else{
        strcpy(staff.name_two,staff.name_one);
    }
    staff.type=FIND;
    if((send(sock_fd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
    while(1){
        if((recv(sock_fd,&staff,sizeof(staff),0))==-1)
            ERR_LOG("recv error");
        if(strcmp(staff.passwd,"查询完毕")==0){
            break;
        }  
        cout <<"查询结果如下:" <<endl;
        cout <<"姓名: " <<staff.name_two <<endl;
        cout <<"年龄: " <<staff.age<<endl;
        cout <<"电话: " <<staff.tel<<endl;
        cout <<"职务: " <<staff.major<<endl;
        cout <<"薪资: " <<staff.salar<<endl;
        cout <<"住址: " <<staff.home<<endl;
    }
}
void quit_system(int sock_fd,sta_t staff){
    staff.type=QUIT;
    if((send(sock_fd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
    cout <<"退出登陆" <<endl;
}
void login_system_ok(int sock_fd,person *p){
    int ch2=0, loop=1;
    while(loop){
        // cout <<"---------------------------------------------------------------------------" <<endl;
        // cout <<"  1.添加员工(root)  2.删除员工(root)  3.修改员工信息  4.查看员工信息  5.退出  " <<endl;
        // cout <<"---------------------------------------------------------------------------" <<endl;
        // cout <<"请输入你的选择>>" <<endl;
        p->person_menu();
        cin >> ch2;
        switch(ch2){
        case 1:
            add_staff(sock_fd,p->staff);
            break;
        case 2:
            delete_staff(sock_fd,p->staff);
            break;
        case 3:
            modify_staff(sock_fd,p->staff);
            break;
        case 4:
            find_staff(sock_fd,p->staff);
            break;
        case 5:
            quit_system(sock_fd,p->staff);
            loop=0;
            break;
        default:
            cout <<"输入错误，请重新输入" <<endl;
            break;
        }
    }
}
int login_system(int sock_fd){
    string name;
    cout <<"请输入用户名>>" <<endl;
    cin >> name;
    person *p=NULL;
    if(name=="root"){
        p=person_factory::create_person("root");
    }else{
        p=person_factory::create_person("staff");
    }
    strcpy(p->staff.name_one,name.c_str());
    cout <<"请输入密码>>" <<endl;
    cin >> p->staff.passwd;
    p->staff.type=LOGIN;
    if((send(sock_fd,&(p->staff),sizeof(sta_t),0))==-1)
        ERR_LOG("send error");
    if((recv(sock_fd,&(p->staff),sizeof(sta_t),0))==-1)
        ERR_LOG("recv error");
    if((strcmp(p->staff.passwd,"登陆成功"))==0){
        cout <<p->staff.passwd <<endl;
        login_system_ok(sock_fd,p);
    }else{
        cout <<p->staff.passwd <<endl;
    }
    return 0;
}
int main(int argc,const char * argv[])
{
    if(argc!=3){
        cout <<"<a.out> <IP> <port>" <<endl;
        exit(EXIT_FAILURE);
    }
    int sock_fd=socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd==-1)
        ERR_LOG("socket error");
    struct sockaddr_in client_addr;
    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(atoi(argv[2]));
    client_addr.sin_addr.s_addr=inet_addr(argv[1]);
    socklen_t client_len=sizeof(client_addr);
    if((connect(sock_fd,(struct sockaddr*)&client_addr,client_len))==-1)
        ERR_LOG("connect error");
    // sta_t staff;
    int ch1=0;
    while(1){
        cout <<"---------------------------------------------------------------------------" <<endl;
        cout <<"-----------------------------欢迎使用员工管理系统--------------------------" <<endl;
        cout <<"-------------------------------1.登陆 2.退出------------------------------" <<endl;
        cout <<"---------------------------------------------------------------------------" <<endl;
        cout <<"请输入你的选择>>" <<endl;
        cin >>ch1;
        switch(ch1){
        case 1:
            login_system(sock_fd);
            break;
        case 2:
            cout <<"退出成功" <<endl;
            close(sock_fd);
            exit(EXIT_SUCCESS);
            break;
        default:
            cout <<"输入错误，请重新输入" <<endl;
            break;
        }
    }
    return 0;
}