// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#ifdef WIN32
    #pragma comment(lib, "lib/x86/libmysql.lib")
#else
    #pragma comment(lib, "lib/x64/libmysql.lib")
#endif

#include "include/mysql.h"

int main(int argc, char* argv[])
{
    char szMysqlServerIP[]      = "192.168.1.100";
    char szMysqlUser[]          = "root";
    char szMysqlPasswd[]        = "123456";
    unsigned int uMysqlPort     = 3306;

    
    MYSQL* pMysql = mysql_init(NULL);

    if (pMysql == NULL)
    {
        goto Exit0;
    }
    
    if (!mysql_real_connect(pMysql, szMysqlServerIP, szMysqlUser, szMysqlPasswd, NULL, uMysqlPort, NULL,0))
    {
        printf("Failed to connect to database: Error: %s\n", mysql_error(pMysql));
    }
    else
    {   
        printf("Connect to MySqlServer %s:%u OK\n", szMysqlServerIP, uMysqlPort);
    }
    
Exit0:
    if (pMysql)
    {
        mysql_close(pMysql);
    }
    return 0;
}

