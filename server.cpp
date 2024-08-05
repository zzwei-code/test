#include <myhead.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <sqlite3.h>
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
sem_t sem;
list<char *>name;
typedef struct CLI{
    struct sockaddr_in clientaddr;
    int acceptfd;
    sqlite3 *staff_db;
}cli_t;
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
int staff_num=0;
sqlite3 *staff_init(sqlite3 **staff_db){
    if((sqlite3_open("staff.db",staff_db))!=SQLITE_OK){
        cout <<"sqlite3_open error" <<endl;
        exit(EXIT_FAILURE);
    }
    cout <<"员工管理系统数据库文件打开成功!!!" <<endl;
    char sqlbuff[256]="create table if not exists staff(name text primary key,passwd text)";
    if((sqlite3_exec(*staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        cout <<"sqlite3_exec error" <<endl;
        exit(EXIT_FAILURE);
    }
    cout <<"员工管理系统的账户表创建成功!!!" <<endl;
    strcpy(sqlbuff,"create table if not exists message(name text primary key,age text,tel text,major text,salar text,home text)");
    if((sqlite3_exec(*staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        cout <<"sqlite3_exec error" <<endl;
    }
    cout <<"员工管理系统的个人信息表创建成功!!!" <<endl;
    strcpy(sqlbuff,"insert into staff values('root','123')");
    if((sqlite3_exec(*staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        //cout <<"sqlite3_exec error" <<endl;
    }
    strcpy(sqlbuff,"insert into message values('root','23','12345678901','计算机科学与技术','50000','北京')");
    if((sqlite3_exec(*staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        //cout <<"sqlite3_exec error" <<endl;
    }
    cout <<"root用户创建成功!!!" <<endl;
    return *staff_db;
}
void login_system(int acceptfd,sta_t staff,sqlite3 *staff_db){
    for(list<char *>::iterator it=name.begin();it!=name.end();it++){
        cout << *it  <<"11111111" << endl;
        if((strcmp(*it,staff.name_one)==0)){
            strcpy(staff.passwd,"用户已经登陆");
            if((send(acceptfd,&staff,sizeof(staff),0))==-1)
                ERR_LOG("send error");
            return ;
        }
    }
    name.push_back(staff.name_one);
    cout <<"登陆后有如下员工在线:" <<endl;
    for(list<char *>::iterator it=name.begin();it!=name.end();it++){
        cout << *it << endl;
    }
    char **result=NULL;
    int row=0, column=0;
    char sqlbuff[512]={0};
    sprintf(sqlbuff,"select * from staff where name='%s' and passwd='%s'",staff.name_one,staff.passwd);
    if((sqlite3_get_table(staff_db,sqlbuff,&result,&row,&column,NULL))!=SQLITE_OK){
        cout <<"登陆sqlite3_get_table error" <<endl;
    }
    if(row==0){
        strcpy(staff.passwd,"用户名或密码错误");
    }else if(row==1){
        strcpy(staff.passwd,"登陆成功");
    }
    if((send(acceptfd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
    sqlite3_free_table(result); 
}
int call_back(void *arg,int column,char **f_value,char **f_name){
    int fd=*(int *)arg;
    sta_t staff_temp;
    sprintf(staff_temp.name_two,"%s",f_value[0]);
    sprintf(staff_temp.age,"%s",f_value[1]);
    sprintf(staff_temp.tel,"%s",f_value[2]);
    sprintf(staff_temp.major,"%s",f_value[3]);
    sprintf(staff_temp.salar,"%s",f_value[4]);
    sprintf(staff_temp.home,"%s",f_value[5]);
    if((send(fd,&staff_temp,sizeof(staff_temp),0))==-1)
        ERR_LOG("send error");
    return 0;
}
void find_staff(int acceptfd,sta_t staff,sqlite3 *staff_db){
    char sqlbuff[512]={0};
    sprintf(sqlbuff,"select * from message where name='%s'",staff.name_two);
    if((sqlite3_exec(staff_db,sqlbuff,call_back,&acceptfd,NULL))!=SQLITE_OK){
        cout <<"查询sqlite3_exec error" <<endl;
    }
    strcpy(staff.passwd,"查询完毕");
    if((send(acceptfd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
}
int call_back2(void *arg,int column,char **f_value,char **f_name){
    int fd=*(int *)arg;
    sta_t staff_temp;
    sprintf(staff_temp.name_two,"%s",f_value[0]);
    
    if((send(fd,&staff_temp,sizeof(staff_temp),0))==-1)
        ERR_LOG("send error");
    return 0;
}
void find_name(int acceptfd,sta_t staff,sqlite3 *staff_db){
    char sqlbuff[512]="select * from staff";
    if((sqlite3_exec(staff_db,sqlbuff,call_back2,&acceptfd,NULL))!=SQLITE_OK){
        cout <<"查询名字sqlite3_exec error" <<endl;
    }
    strcpy(staff.passwd,"查询完毕");
    if((send(acceptfd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
}
void add_staff(int acceptfd,sta_t staff,sqlite3 *staff_db){
    char sqlbuff[512]={0};
    sprintf(sqlbuff,"insert into staff values('%s','%s')",staff.name_two,staff.passwd);
    if((sqlite3_exec(staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        cout <<"添加(staff)sqlite3_exec error" <<endl;
    }
    sprintf(sqlbuff,"insert into message values('%s','%s','%s','%s','%s','%s')",staff.name_two,staff.age,staff.tel,staff.major,staff.salar,staff.home);
    if((sqlite3_exec(staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        cout <<"添加(message)sqlite_exec error" <<endl;
        strcpy(staff.passwd,"添加失败");
    }else{
        strcpy(staff.passwd,"添加成功");
    }
    if((send(acceptfd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
}
void delete_staff(int acceptfd,sta_t staff,sqlite3 *staff_db){
    char sqlbuff[512]={0};
    sprintf(sqlbuff,"delete from staff where name='%s'",staff.name_two);
    if((sqlite3_exec(staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        cout <<"删除(staff)sqlite3_exec error" <<endl;
    }
    sprintf(sqlbuff,"delete from message where name='%s'",staff.name_two);
    if((sqlite3_exec(staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        cout <<"删除(message)sqlite3_exec error" <<endl;
        strcpy(staff.passwd,"删除失败");
    }else{
        strcpy(staff.passwd,"删除成功");
    }
    if((send(acceptfd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
}
void modify_staff(int acceptfd,sta_t staff,sqlite3 *staff_db){
    char sqlbuff[512]={0};
    sprintf(sqlbuff,"update message set age='%s',tel='%s',major='%s',salar='%s',home='%s' where name='%s'",staff.age,staff.tel,staff.major,staff.salar,staff.home,staff.name_two);
    if((sqlite3_exec(staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        cout <<"修改sqlite3_exec error" <<endl;
        strcpy(staff.passwd,"修改失败");
    }else{
        strcpy(staff.passwd,"修改成功");
    }
    if((send(acceptfd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
}
void modify_staff2(int acceptfd,sta_t staff,sqlite3 *staff_db){
    char sqlbuff[512]={0};
    sprintf(sqlbuff,"update message set age='%s',tel='%s',home='%s' where name='%s'",staff.age,staff.tel,staff.home,staff.name_two);
    if((sqlite3_exec(staff_db,sqlbuff,NULL,NULL,NULL))!=SQLITE_OK){
        cout <<"修改sqlite3_exec error" <<endl;
        strcpy(staff.passwd,"修改失败");
    }else{
        strcpy(staff.passwd,"修改成功");
    }
    if((send(acceptfd,&staff,sizeof(staff),0))==-1)
        ERR_LOG("send error");
}
void quit_system(sta_t staff,sqlite3 *staff_db){
    for(list<char *>::iterator it=name.begin();it!=name.end();it++){
        if((strcmp(staff.name_one,*it))==0){
            name.erase(it);
            break;
        }
    }
    cout <<"退出后有如下员工在线:" <<endl;
    for(list<char *>::iterator it=name.begin();it!=name.end();it++){
        cout << *it << endl;
    }
}
void *task_func(void *arg){
    pthread_detach(pthread_self());
    cli_t cli=*(cli_t *)arg;
    sem_post(&sem);
    cout <<"客户端{%s}:{%d}连接成功"<<inet_ntoa(cli.clientaddr.sin_addr) <<ntohs(cli.clientaddr.sin_port);
    int ret=0;
    sta_t staff;
    while(1){
        if((ret=recv(cli.acceptfd,&staff,sizeof(staff),0))==-1){
            ERR_LOG("recv error");
        }else if(ret==0){
            cout <<"客户端{%s}:{%d}连接断开" <<inet_ntoa(cli.clientaddr.sin_addr) <<ntohs(cli.clientaddr.sin_port);
            break;
        }
        // 双名字系统是为了执行登陆成功界面的操作
        // 因为使用一个名字，登陆成功之后，执行其他函数时名字会被修改
        // 这样就不再是root用户，执行不了全部的函数
        if((strncmp(staff.name_one,"root",4))==0){
            //通过名字判断是什么用户，执行什么函数
            switch(staff.type){
            case LOGIN:
                login_system(cli.acceptfd,staff,cli.staff_db);
                break;
            case FIND:
                find_staff(cli.acceptfd,staff,cli.staff_db);
                break;
            case ADD:
                add_staff(cli.acceptfd,staff,cli.staff_db);
                break;
            case DELETE:
                delete_staff(cli.acceptfd,staff,cli.staff_db);
                break;
            case MODIFY:
                modify_staff(cli.acceptfd,staff,cli.staff_db);
                break;
            case QUIT:
                quit_system(staff,cli.staff_db);
                break;
            case FIND_NAME:
                find_name(cli.acceptfd,staff,cli.staff_db);
                break;
            }
        }else{
            switch(staff.type){
            case LOGIN:
                login_system(cli.acceptfd,staff,cli.staff_db);
                break;
            case FIND:
                find_staff(cli.acceptfd,staff,cli.staff_db);
                break;
            case MODIFY:
                modify_staff2(cli.acceptfd,staff,cli.staff_db);
                break;
            case QUIT:
                quit_system(staff,cli.staff_db);
                break;
            }
        }
    }
    close(cli.acceptfd);
    pthread_exit(NULL);
}
int main(int argc,const char * argv[])
{
    int sock_fd=socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd==-1)
        ERR_LOG("socket error");
    cli_t cli;
    staff_init(&cli.staff_db);;
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(atoi("9999"));
    server_addr.sin_addr.s_addr=inet_addr("192.168.250.100");
    socklen_t server_len=sizeof(server_addr);
    if((bind(sock_fd,(struct sockaddr*)&server_addr,server_len))==-1)
        ERR_LOG("bind error");
    if((listen(sock_fd,10))==-1)
        ERR_LOG("listen error");
    struct sockaddr_in client_addr;
    socklen_t client_len=sizeof(client_addr);
    sem_init(&sem,0,1);
    int accept_fd=0;
    pthread_t tid=0;
    while(1){
        cout <<"正在等待客户端连接..." <<endl;
        if((accept_fd=accept(sock_fd,(struct sockaddr*)&client_addr,&client_len))==-1)
            ERR_LOG("accept error");
        cli.acceptfd=accept_fd;
        cli.clientaddr=client_addr;
        if((pthread_create(&tid,NULL,task_func,&cli))!=0){
            cout <<"pthread create error" <<endl;
        }
    }
    sem_destroy(&sem);
    close(sock_fd);
    return 0;
}