// Shell
#include "types.h"
#include "user.h"
#include "fcntl.h"




enum cmd_type { NORMAL, REDIR, PIPE, PARALLEL, AND, OR, FILE};

typedef struct normal_cmd{
  enum cmd_type type;
  char **argv;//to store aruguments
  char *ip_file, *op_file;
} normal_cmd;


typedef struct complex_cmd{
  enum cmd_type type;
  normal_cmd *left_cmd, *right_cmd;//to two different commands
} complex_cmd;

int fetchcmd(char *cmd, int size)
{
  printf(1, "\nMyShell>");
  gets(cmd, size);
  //printf(1, "inside fetch == %s",cmd);
  if(cmd[0] == 0) // EOF
    return -1;
  return 0;
}

int validate(char **argv)
{
    if(strcmp(argv[0],"ls")==0 || strcmp(argv[0],"cat")==0 || strcmp(argv[0],"grep")==0 || strcmp(argv[0],"echo")==0 || strcmp(argv[0],"wc")==0)
    {
      return 0;
    }
    else if(strcmp(argv[0],"ps")==0)
    {
      if (argv[1] == 0)
        {
            return 0;
        }
      else
        return -1;
    }

    else if(strcmp(argv[0],"procinfo")==0)
    {
      if (argv[2] ==0 && argv[1] != 0 )
      {
            for (int i=0; i< strlen(argv[1]); i++)
            { 
            if(argv[1][i] < 48 || argv[1][i] > 57)
                {
                    return -1;            
                }
             }
            if(atoi(argv[1]) >= 0)
            {
                return 0;
            }
            else
            {
                return -1;
            }
      }
      else
        return -1;
    }
    else
    {
      return -1;
    }
}

void rstrip(char *s)
{
  int i = strlen(s);
  //printf(1,"\n%d ==== %s === %c",i,s,s[i-1]);
  while(s[--i] == ' ');
  s[++i] = '\0';
  //printf(1,"\n%d === %c",i,s[i-1]);
  //printf(1,"\n%d ==== %s === %c",strlen(s),s,s[strlen(s)-1]);
}

int parse_normal_cmd2(char *cmd,normal_cmd *nor_cmd)
{
  nor_cmd->type = NORMAL;
  nor_cmd->ip_file[0]='\0';
  nor_cmd->op_file[0]='\0';

  char *token, redir_op = '\0';
  int i, j, f = 0, space;
  token = malloc(sizeof(char) * 1024);

  //printf(1,"\n[82]CMD RECVD%s\n", cmd);
  j = 0;
  space = 0;
  for (i=0;i<strlen(cmd);i++)
  {
    if(cmd[i] != '<' && cmd[i] != '>' && cmd[i] != '\n')
    {
        if(cmd[i] == ' ')
        {
            space = 1;
        }
        else if(cmd[i] != ' ' && space ==  0 )
        {
            token[j++] = cmd[i];
        }
        else
        {
            rstrip(token);
            strcpy(nor_cmd->argv[f],token);
            memset(token, '\0', 1024);
            j=0;
            f++;
            token[j++] = cmd[i];
            space = 0;
            //printf(1,"\n [105] Stored \t %s \t at  %d",nor_cmd->argv[f-1],f-1);
        }
    }

    else if (cmd[i]=='\n')
    {
        rstrip(token);
        strcpy(nor_cmd->argv[f],token);

        free(token);
        j=0;
        //printf(1,"\n [116] Stored \t %s \t at  %d",nor_cmd->argv[f],f);
        nor_cmd->argv[++f] = 0;
        return 1;
    }
    else
    {
        rstrip(token);
        strcpy(nor_cmd->argv[f],token);
        memset(token, '\0', 1024);
        j=0;
        nor_cmd->argv[++f] = 0;
        //redir = 1;
        redir_op = cmd[i];
        nor_cmd->type = REDIR;
        break;
    }
  }

  //printf(1,"\n[134] REDIR_OP = %c",redir_op);

  while(cmd[++i] == ' '){};

  //printf(1,"\n[138] cmd[i-1] = %c\t cmd[i] = %c\t cmd[i+1] = %c\t ",cmd[i-1],cmd[i],cmd[i+1]);

  while(cmd[i] != ' ' && cmd[i] != '<' && cmd[i] != '>' && cmd[i] != '\n')
  {
      token[j++] = cmd[i++];
  }

  rstrip(token);
  if(redir_op == '<')
  {
      strcpy(nor_cmd->ip_file,token);
      //printf(1,"\n[149] Fetched input file %s",nor_cmd->ip_file);
  }
  else
  {
      strcpy(nor_cmd->op_file, token);
      //printf(1,"\n[154] Fetched output file %s",nor_cmd->op_file);
  }
  memset(token, '\0', 1024);
  j=0;


  //printf(1,"\n[160] cmd[i-1] = %c\t cmd[i] = %c\t cmd[i+1] = %c\t ",cmd[i-1],cmd[i],cmd[i+1]);


  while(cmd[i] == ' ')
  {
      i++;
      //printf(1,"\n[165] Multiple Space found!!! next char = %c",cmd[i+1]);
  }




  //printf(1,"\n[168] cmd[i-1] = %c\t cmd[i] = %c\t cmd[i+1] = %c\t ",cmd[i-1],cmd[i],cmd[i+1]);

  if(cmd[i] == '\n')
  {
      return 1;
  }
  else
  {
            if(cmd[i] == '<')
            {
                redir_op = '<';
            }
            else
            {
                redir_op = '>';
            }
            i++;
            //printf(1,"\n[185] cmd[i-1] = %c\t cmd[i] = %c\t cmd[i+1] = %c\t ",cmd[i-1],cmd[i],cmd[i+1]);

            while(cmd[i] == ' ')
             {
                 i++;
             };
            //printf(1,"\n[191] cmd[i-1] = %c\t cmd[i] = %c\t cmd[i+1] = %c\t ",cmd[i-1],cmd[i],cmd[i+1]);

            while(cmd[i] != ' ' && cmd[i] != '\n')
            {
                token[j++]=cmd[i++];
            }
            rstrip(token);
            if(redir_op == '<')
            {
                strcpy(nor_cmd->ip_file, token);
                //printf(1,"\n[201] Fetched input file %s",nor_cmd->ip_file);
            }
            else
            {
                strcpy(nor_cmd->op_file, token);
                //printf(1,"\n[206] Fetched output file %s",nor_cmd->op_file);

            }
            return 1;
  }
}

enum cmd_type get_cmd_type(char * cmd)
{
    int i=0;
    char *temp;
    temp = malloc(sizeof(char) * 1024);
    strcpy(temp,cmd);
    temp[16] = '\0';
    if (strcmp(temp,"executeCommands ") == 0)
        return FILE;
    for(i=0; i< strlen(cmd) ; i++)
    {
        if(cmd[i+1] == '|')
        {
            if(cmd[i+1] == '|' && cmd[i+2] == '|')
                {
                    return OR;
                }
            else
                {
                    return PIPE;
                }

        }
        if(cmd[i+1] == ';')
        {
            return PARALLEL;
        }
        if (cmd[i+1] == '&' && cmd[i+2] == '&')
        {
            return AND;
        }
    }
    return NORMAL;
}

int run_normal_cmd(char *cmd,normal_cmd *nor_cmd, int *status)
{
        int flag;


          flag = validate(nor_cmd->argv);
          if (flag== -1)
            {
              printf(1,"\nIllegal command or arguments\n");
              *status = -1;
              return -1;
            }
          else
          {
            if(nor_cmd->type == NORMAL)
              {
                //printf(1,"\n[458]Inside Normal Execution.\n");
                if(fork()==0)
                {
                    if(strcmp(nor_cmd->argv[0], "ps")==0)
                    {
                        psinfo();
                    }
                    else
                    {
                        exec(nor_cmd->argv[0],nor_cmd->argv);
                    }
                    exit(0);
                }
                wait(status);
              }
              else if(nor_cmd->type == REDIR)
              {
                //printf(1,"\n[475]Inside Redirection Execution.\n");
                if(fork()==0)
                {
                    if(nor_cmd->ip_file[0] != '\0')
                    {
                      //printf(1,"\n[480]Found INput redirection to %s.\n",nor_cmd->ip_file);
                      close(0);
                      int fd;
                      if ((fd = open(nor_cmd->ip_file, O_RDONLY)) < 0)
                      {
                          printf(2,"\nError opening input file!!\n");
                          exit(-1);
                      }
                    }
                    if(nor_cmd->op_file[0] != '\0')
                    {
                      //printf(1,"\n[486]Found OUTput redirection to %s.\n",nor_cmd->op_file);
                      int fd;   
                    if ((fd =open(nor_cmd->op_file, O_CREATE|O_WRONLY)) >= 0)
                    {
                        close(fd);
                        char *rm_cmd[10]; 
                        rm_cmd[0] = "rm";
                        rm_cmd[1] = nor_cmd->op_file;
                        rm_cmd[2] = 0;
                        // printf(1,"\n[332] rm_cmd[0] = !%s!\trm_cmd[1] = !%s!\trm_cmd[2] = !%s!\t",rm_cmd[0],rm_cmd[1],rm_cmd[2]);
                        if(fork()==0)
                        {
                            exec("rm",rm_cmd);          //Removing file and reopening, same as truncating
                            exit(0);
                        }
                        wait(0);
                        // printf(1,"\n[342] REACHED HERE");
                        close(1);
                        open(nor_cmd->op_file, O_CREATE|O_WRONLY);
                    }
                    else
                    {
                        printf(2,"\nError opening output file!!\n");
                        exit(-1);
                    }
                    }
                    exec(nor_cmd->argv[0],nor_cmd->argv);
                    exit(0);
                }
                wait(status);
              }

          }
          //printf(1,"\n\n[319]####Status of CHILD = %d",*status);
          return 1;
}

int exec_normal_cmd(normal_cmd *cmd , int *status)
{
    int flag;
    flag = validate(cmd->argv);
    if (flag==-1)
      {
        printf(1,"Illegal command or arguments");
        *status = -1;
        return -1;
      }
    else
    {
      if(cmd->type == NORMAL)
        {

            if(strcmp(cmd->argv[0], "ps")==0)
            {
                psinfo();
            }
            else
            {
                *status = exec(cmd->argv[0],cmd->argv);
            }
            exit(0);
            return 0;
        }
        else
        {
                //printf(1,"\n[475]Inside Redirection Execution.\n");
                if(fork()==0)
                {
                    if(cmd->ip_file[0] != '\0')
                    {
                      //printf(1,"\n[480]Found INput redirection to %s.\n",nor_cmd->ip_file);
                      close(0);
                      int fd;
                      if ((fd = open(cmd->ip_file, O_RDONLY)) < 0)
                      {
                          printf(2,"\nError opening input file!!\n");
                          exit(-1);
                          return -1;
                      }
                    }
                    if(cmd->op_file[0] != '\0')
                    {
                      //printf(1,"\n[486]Found OUTput redirection to %s.\n",cmd->op_file);
                      int fd;   
                    if ((fd =open(cmd->op_file, O_CREATE|O_WRONLY)) >= 0)
                    {
                        close(fd);
                        char *rm_cmd[10]; 
                        rm_cmd[0] = "rm";
                        rm_cmd[1] = cmd->op_file;
                        rm_cmd[2] = 0;
                        // printf(1,"\n[332] rm_cmd[0] = !%s!\trm_cmd[1] = !%s!\trm_cmd[2] = !%s!\t",rm_cmd[0],rm_cmd[1],rm_cmd[2]);
                        if(fork()==0)
                        {
                            exec("rm",rm_cmd);          //Removing file and reopening, same as truncating
                            exit(0);
                            return 0;
                        }
                        wait(0);
                        // printf(1,"\n[342] REACHED HERE");
                        close(1);
                        open(cmd->op_file, O_CREATE|O_WRONLY);
                    }
                    else
                    {
                        printf(2,"\nError opening output file!!\n");
                        exit(-1);
                        return -1;
                    }
                    }
                    exec(cmd->argv[0],cmd->argv);
                    exit(0);
                    return 0;
                }
                wait(status);
        }
    }
    return 0;
}

enum cmd_type split_cmd(char *cmd, char **cmds,enum cmd_type type)
{
    int i=0;
     type = NORMAL;
    for(i=0; i< strlen(cmd) ; i++)
    {
        if(cmd[i+1] == '|')
        {
            if(cmd[i+1] == '|' && cmd[i+2] == '|')
            {
                type = OR;
                i ++;
                break;
            }
            else{
                type = PIPE;
                i ++;
                break;
            }
            
        }
        if(cmd[i+1] == ';')
        {
                type = PARALLEL;
                i ++;
                break;
        }
        if (cmd[i+1] == '&' && cmd[i+2] == '&')
        {
            type = AND;
            i ++;
            break;
        }
    }

    if(type == PIPE || type == PARALLEL)
    {
        strcpy(cmds[0],cmd);
        cmds[0][i] = '\n';
        cmds[0][i+1] = '\0';
        strcpy(cmds[1],cmd+i+1);
        int i=0;
        if(cmds[1][i]==' ')
        {
            while(cmds[1][++i]==' ');
        }
        cmds[1] = cmds[1] + i;
    }
    else
    {
        strcpy(cmds[0],cmd);
        cmds[0][i] = '\n';
        cmds[0][i+1] = '\0';
        strcpy(cmds[1],cmd+i+2);
        int i=0;
        if(cmds[1][i]==' ')
        {
            while(cmds[1][++i]==' ');
        }
        cmds[1] = cmds[1] + i;
    }

    //printf(1,"\n[426]LEFT_CMD = !!%s!!\nRIGHT_CMD =!!%s!!",cmds[0],cmds[1]);
    return type;
}

int run_parallel_cmd(char *cmd,complex_cmd *prl_cmd, int *status)
{
    char *cmds[2];
    cmds[0] = malloc(sizeof(char) * 1024);
    cmds[1] = malloc(sizeof(char) * 1024);

    prl_cmd->type = split_cmd(cmd,cmds,prl_cmd->type);
    char *left_cmd = malloc(sizeof(char) * 1024);
    strcpy(left_cmd,cmds[0]);
    char *right_cmd = malloc(sizeof(char) * 1024);
    strcpy(right_cmd,cmds[1]);

    //printf(1,"\n[399]LEFT_CMD ==%s\nRIGHT_CMD ==%s",left_cmd,right_cmd);
    int left,right;
    left = fork();
    if (left == 0)
    {
        parse_normal_cmd2(left_cmd,prl_cmd->left_cmd);
        *status = exec_normal_cmd(prl_cmd->left_cmd,status);
    }

    right = fork();
    if (right == 0)
    {
        parse_normal_cmd2(right_cmd,prl_cmd->right_cmd);
        *status = exec_normal_cmd(prl_cmd->right_cmd,status);
    }

    int a,b;
    a=wait(status);
    b=wait(status);
    if((a==left || a==right) && (b==left || b==right))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int run_pipe_cmd(char *cmd,complex_cmd *pipe_cmd, int *status)
{
    char *cmds[2];
    cmds[0] = malloc(sizeof(char) * 1024);
    cmds[1] = malloc(sizeof(char) * 1024);

    pipe_cmd->type = split_cmd(cmd,cmds,pipe_cmd->type);
    char *left_cmd = malloc(sizeof(char) * 1024);
    strcpy(left_cmd,cmds[0]);
    char *right_cmd = malloc(sizeof(char) * 1024);
    strcpy(right_cmd,cmds[1]);


    int pp[2];
    pipe(pp);

    //parse_normal_cmd2(cmd,nor_cmd);

    if(fork() == 0)
    {
        parse_normal_cmd2(left_cmd,pipe_cmd->left_cmd);
        close(1);
        dup(pp[1]);
        close(pp[0]);
        close(pp[1]);

        exec_normal_cmd(pipe_cmd->left_cmd,status);
        exit(0);
    }
    if(fork() == 0)
    {
        parse_normal_cmd2(right_cmd,pipe_cmd->right_cmd);
        close(0);
        dup(pp[0]);
        close(pp[0]);
        close(pp[1]);

        *status = exec_normal_cmd(pipe_cmd->right_cmd,status);
        exit(0);
    }
    close(pp[0]);
    close(pp[1]);
    wait(0);
    wait(0);

    return 1;
}

int run_cond_cmd(char *cmd,complex_cmd *cond_cmd, int *status)
{
    char *cmds[2];
    cmds[0] = malloc(sizeof(char) * 1024);
    cmds[1] = malloc(sizeof(char) * 1024);

    cond_cmd->type = split_cmd(cmd,cmds,cond_cmd->type);
    char *left_cmd = malloc(sizeof(char) * 1024);
    strcpy(left_cmd,cmds[0]);
    char *right_cmd = malloc(sizeof(char) * 1024);
    strcpy(right_cmd,cmds[1]);

    //printf(1,"\n[399]LEFT_CMD ==%s\nRIGHT_CMD ==%s",left_cmd,right_cmd);
    int left;//,right;
    left = fork();
    if (left == 0)
    {
        parse_normal_cmd2(left_cmd,cond_cmd->left_cmd);
        exec_normal_cmd(cond_cmd->left_cmd,status);
    }
    wait(status);
    // printf(1,"\n[543] STATUS OF LEFT == %d",*status);

    if((cond_cmd->type == AND && *status == 0) || (cond_cmd->type == OR && *status == -1) )
    {
        int right = fork();
        if (right == 0)
        {
            parse_normal_cmd2(right_cmd,cond_cmd->right_cmd);
            *status = exec_normal_cmd(cond_cmd->right_cmd,status);
        }
        wait(status);
        return 0;
    }
    return 0;
}

void run_file_cmds(char *cmd)
{
    //printf(1,"\nInside run_file_cmds");
    int i=15;
    //printf(1,"\n[556]FileName =%s",cmd+15);
    while(cmd[++i]==' '){};
    char  *file = cmd+i;
    file[strlen(file)-1]='\0';
    //printf(1,"\n[559]FileName =!!%s!!",file);


    int fd = open(file,O_RDWR);
    //printf(1,"\nFD == %d",fd);
    if(fd == -1)
    {
        return;
    }

    //char ch;
    int num,j=0, f=0;
    char buf[2048];
    num = read(fd, buf ,sizeof(buf));
    // printf(1,"\nBytes read== %d", num);
    close(fd);
    // printf(1,"\n FILE = %s\n",buf);
    i=0;
    char **cmds = malloc(sizeof(char *) * 100);
    char *ln = malloc(sizeof(char) * 1024);
    while(i != num)
    {
        if(buf[i] != '\n')
        {
            ln[j++] = buf[i];
        }
        else
        {
            ln[j++] = buf[i];
            ln[j] = '\n';
            ln[++j] = '\0';
            cmds[f] = malloc(sizeof(char) * 1024);
            strcpy(cmds[f],ln);
            f++;
            // printf(1,"##LN = %s\n cmds inserted ===%s",ln, cmds[f-1]);

            free(ln);
            ln = malloc(sizeof(char) * 1024);
            j=0;
        }
        //ch = buf[i];
        //printf(1,"%c",ch);
        i++;
    }
    int x = 0;
    int *status = malloc(sizeof(int));

    while(x < f)
    {
        // printf(1,"\n\n$$ F= %d\t x = %d",f,x);
        memset(cmd, '\0', sizeof(cmd));
        cmd = cmds[x];
        if(cmd[0]=='\n')
        {
            i++;
            continue;
        }


        enum cmd_type type = get_cmd_type(cmd);

        if (type == NORMAL || type == REDIR)
        {
            normal_cmd *nor_cmd;
            nor_cmd = malloc(sizeof(normal_cmd));
            nor_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            nor_cmd->ip_file = malloc(sizeof(char) * 1024);
            nor_cmd->op_file = malloc(sizeof(char) * 1024);
            for(i=0;i<10;i++)
            {
              nor_cmd->argv[i] = malloc(sizeof(char) * 1024);
            }
            // printf(1,"[365]CMD SENT TO RUN CMD === %s\n", cmd);
            parse_normal_cmd2(cmd,nor_cmd);
            run_normal_cmd(cmd,nor_cmd,status);
            // printf(1,"\n\n[605]####Status of CHILD = %d",*status);
        }
        //enum cmd_type { NORMAL, REDIR, PIPE, PARALLEL, AND, OR, FILE};

        else if(type == PIPE)
        {
            // printf(1,"\n[373]FOUND PIPE CMD === %s",cmd);
            complex_cmd *pipe_cmd;
            pipe_cmd = malloc(sizeof(complex_cmd));
            pipe_cmd->left_cmd = malloc(sizeof(normal_cmd));
            pipe_cmd->right_cmd = malloc(sizeof(normal_cmd));
            pipe_cmd->left_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            pipe_cmd->left_cmd->ip_file = malloc(sizeof(char) * 1024);
            pipe_cmd->left_cmd->op_file = malloc(sizeof(char) * 1024);
            pipe_cmd->right_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            pipe_cmd->right_cmd->ip_file = malloc(sizeof(char) * 1024);
            pipe_cmd->right_cmd->op_file = malloc(sizeof(char) * 1024);

            for(i=0;i<10;i++)
            {
              pipe_cmd->left_cmd->argv[i] = malloc(sizeof(char) * 1024);
              pipe_cmd->right_cmd->argv[i] = malloc(sizeof(char) * 1024);
            }
            run_pipe_cmd(cmd,pipe_cmd,status);

        }
        else if(type == PARALLEL)
        {
            // printf(1,"\n[373]FOUND PARALLEL CMD === %s",cmd);
            complex_cmd *prl_cmd;
            prl_cmd = malloc(sizeof(complex_cmd));
            prl_cmd->left_cmd = malloc(sizeof(normal_cmd));
            prl_cmd->right_cmd = malloc(sizeof(normal_cmd));
            prl_cmd->left_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            prl_cmd->left_cmd->ip_file = malloc(sizeof(char) * 1024);
            prl_cmd->left_cmd->op_file = malloc(sizeof(char) * 1024);
            prl_cmd->right_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            prl_cmd->right_cmd->ip_file = malloc(sizeof(char) * 1024);
            prl_cmd->right_cmd->op_file = malloc(sizeof(char) * 1024);

            for(i=0;i<10;i++)
            {
              prl_cmd->left_cmd->argv[i] = malloc(sizeof(char) * 1024);
              prl_cmd->right_cmd->argv[i] = malloc(sizeof(char) * 1024);
            }
            run_parallel_cmd(cmd,prl_cmd,status);

        }
        else if(type == AND || type == OR)
        {
            //printf(1,"\n[373]FOUND PIPE AND === %s",cmd);

            //printf(1,"\n[373]FOUND OR CMD === %s",cmd);

            complex_cmd *cond_cmd;
            cond_cmd = malloc(sizeof(complex_cmd));
            cond_cmd->left_cmd = malloc(sizeof(normal_cmd));
            cond_cmd->right_cmd = malloc(sizeof(normal_cmd));
            cond_cmd->left_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            cond_cmd->left_cmd->ip_file = malloc(sizeof(char) * 1024);
            cond_cmd->left_cmd->op_file = malloc(sizeof(char) * 1024);
            cond_cmd->right_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            cond_cmd->right_cmd->ip_file = malloc(sizeof(char) * 1024);
            cond_cmd->right_cmd->op_file = malloc(sizeof(char) * 1024);

            for(i=0;i<10;i++)
            {
              cond_cmd->left_cmd->argv[i] = malloc(sizeof(char) * 1024);
              cond_cmd->right_cmd->argv[i] = malloc(sizeof(char) * 1024);
            }

            run_cond_cmd(cmd,cond_cmd,status);
        }
        x++;
        // printf(1,"\n[705]REACHED HERE!!!\nF=%d\t x %d",f,x);

    }
    return;
}

int
main(void)
{
  char *cmd ;
  int size = 1024, i, *status;//,status
  cmd = malloc(sizeof(char) * size);
  status = malloc(sizeof(int));

  i=0;

  //printf(1,"\n%s==%s\n%s==%s\n%d==%d\n",argv[0],argv2[0],argv[1],argv2[1],argv[2],argv2[2]);
  while(1)
  {
    if(fetchcmd(cmd,size) == -1)
    {
        continue;
    }
    // printf(1,"\ncmd === %s\n", cmd);
    if(cmd[0]=='e' && cmd[1]=='x' && cmd[2]=='i' && cmd[3]=='t')
      {
          int a = 4;
          if(cmd[a] == ' ')
          {
              while(cmd[++a] == ' ') {};
          }
          if (cmd[a] == '\n')
          {
              exit(0);
          }
          else
          {
              printf(1,"\nIllegal command or arguments\n");
              continue;
          }
          
      }
    else
    {
        //printf(1,"[361]CMD SENT TO get_cmd_type === %s\n", cmd);

        enum cmd_type type = get_cmd_type(cmd);

        if (type == NORMAL || type == REDIR)
        {
            normal_cmd *nor_cmd;
            nor_cmd = malloc(sizeof(normal_cmd));
            nor_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            nor_cmd->ip_file = malloc(sizeof(char) * 1024);
            nor_cmd->op_file = malloc(sizeof(char) * 1024);
            for(i=0;i<10;i++)
            {
              nor_cmd->argv[i] = malloc(sizeof(char) * 1024);
            }
            // printf(1,"[365]CMD SENT TO RUN CMD === %s\n", cmd);
            parse_normal_cmd2(cmd,nor_cmd);
            run_normal_cmd(cmd,nor_cmd,status);
            // printf(1,"\n\n[605]####Status of CHILD = %d",*status);
        }
        //enum cmd_type { NORMAL, REDIR, PIPE, PARALLEL, AND, OR, FILE};

        else if(type == PIPE)
        {
            // printf(1,"\n[373]FOUND PIPE CMD === %s",cmd);
            complex_cmd *pipe_cmd;
            pipe_cmd = malloc(sizeof(complex_cmd));
            pipe_cmd->left_cmd = malloc(sizeof(normal_cmd));
            pipe_cmd->right_cmd = malloc(sizeof(normal_cmd));
            pipe_cmd->left_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            pipe_cmd->left_cmd->ip_file = malloc(sizeof(char) * 1024);
            pipe_cmd->left_cmd->op_file = malloc(sizeof(char) * 1024);
            pipe_cmd->right_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            pipe_cmd->right_cmd->ip_file = malloc(sizeof(char) * 1024);
            pipe_cmd->right_cmd->op_file = malloc(sizeof(char) * 1024);

            for(i=0;i<10;i++)
            {
              pipe_cmd->left_cmd->argv[i] = malloc(sizeof(char) * 1024);
              pipe_cmd->right_cmd->argv[i] = malloc(sizeof(char) * 1024);
            }
            run_pipe_cmd(cmd,pipe_cmd,status);

        }
        else if(type == PARALLEL)
        {
            // printf(1,"\n[373]FOUND PARALLEL CMD === %s",cmd);
            complex_cmd *prl_cmd;
            prl_cmd = malloc(sizeof(complex_cmd));
            prl_cmd->left_cmd = malloc(sizeof(normal_cmd));
            prl_cmd->right_cmd = malloc(sizeof(normal_cmd));
            prl_cmd->left_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            prl_cmd->left_cmd->ip_file = malloc(sizeof(char) * 1024);
            prl_cmd->left_cmd->op_file = malloc(sizeof(char) * 1024);
            prl_cmd->right_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            prl_cmd->right_cmd->ip_file = malloc(sizeof(char) * 1024);
            prl_cmd->right_cmd->op_file = malloc(sizeof(char) * 1024);

            for(i=0;i<10;i++)
            {
              prl_cmd->left_cmd->argv[i] = malloc(sizeof(char) * 1024);
              prl_cmd->right_cmd->argv[i] = malloc(sizeof(char) * 1024);
            }
            run_parallel_cmd(cmd,prl_cmd,status);

        }
        else if(type == AND || type == OR)
        {
            //printf(1,"\n[373]FOUND PIPE AND === %s",cmd);

            //printf(1,"\n[373]FOUND OR CMD === %s",cmd);

            complex_cmd *cond_cmd;
            cond_cmd = malloc(sizeof(complex_cmd));
            cond_cmd->left_cmd = malloc(sizeof(normal_cmd));
            cond_cmd->right_cmd = malloc(sizeof(normal_cmd));
            cond_cmd->left_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            cond_cmd->left_cmd->ip_file = malloc(sizeof(char) * 1024);
            cond_cmd->left_cmd->op_file = malloc(sizeof(char) * 1024);
            cond_cmd->right_cmd->argv = malloc(sizeof(char*) * 10);//to store aruguments
            cond_cmd->right_cmd->ip_file = malloc(sizeof(char) * 1024);
            cond_cmd->right_cmd->op_file = malloc(sizeof(char) * 1024);

            for(i=0;i<10;i++)
            {
              cond_cmd->left_cmd->argv[i] = malloc(sizeof(char) * 1024);
              cond_cmd->right_cmd->argv[i] = malloc(sizeof(char) * 1024);
            }

            run_cond_cmd(cmd,cond_cmd,status);

        }
        else if(type == FILE)
        {
            // printf(1,"\n[373]FOUND FILE CMD === %s",cmd);
            run_file_cmds(cmd);
        }
    }

    free(cmd);
    cmd = malloc(sizeof(char) * size);

  }


  exit(0);
}
