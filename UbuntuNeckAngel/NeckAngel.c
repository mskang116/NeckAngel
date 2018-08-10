#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int getProcess(int *pid, char *command, char *flag) {
	int i = 0;
	int stdout_bk;
	int pipefd[2];
	char command_line[255];
	char *token;

	stdout_bk = dup(fileno(stdout));
	pipe(pipefd);
	dup2(pipefd[1], fileno(stdout));
	
	system("ps -e -o pid,command | grep NeckAngel | (head -n1 && tail -n1)");
	fflush(stdout);
	close(pipefd[1]);
	read(pipefd[0], command_line, 255);
	
	dup2(stdout_bk, fileno(stdout));
	close(pipefd[0]);
	
	token = strtok(command_line, " ");
	
	while(token != NULL)
	{
		if(i < 1){
			*pid = atoi(token);
		}
		else if(i < 2) {
			strcpy(command, token);
		}
		else {
			strcpy(flag, token);
		}
		token = strtok(NULL, " ");
		i++;
	}
	
	return 1;
}

int main(int argc, char **argv)
{
	int pid = -1;
	int status;
	char command[255];
	char flag[10];
	if(argc > 1) {
		if(strcmp(argv[1], "-k") == 0) 
		{	
			if(getProcess(&pid, command, flag)) 
			{
				printf("pid : %d command : %s flag : %s\n", pid, command, flag);
				if(strcmp(flag, "-k\n") == 0) {
					printf("Program Not Found!\n");
					exit(1);
				} else {
					puts("Kill the Angel!");
					sprintf(command, "kill %d", pid);
					system(command);
				}
			}
			printf("Kill NeckAngel Process\n");
			exit(0);
		} 
		else
		{
			puts("Usage : ./NeckAngel or ./NeckAngel -k");
			exit(1);
		}
	}
	else 
	{
		if(getProcess(&pid, command, flag))
		{
			if(getpid() != pid) {
				puts("이미 척추요정이 살아있습니다!");
				puts("척추요정을 죽이고 싶으시면 ./NeckAngel -k를 입력해주세요");
				exit(0);
			}
		}
	}

	pid = fork();
	if(pid < 0){
		printf("fork Error... : return is [%d] \n", pid );
		perror("fork error : ");
		exit(0);
	}else if (pid > 0){
		exit(0);
	}else if(pid == 0){
		printf("척추 요정 프로그램 시작\n");
	}

	// 터미널 종료 시 signal의 영향을 받지 않도록 처리
	signal(SIGHUP, SIG_IGN);
	close(0);
	close(1);
	close(2);

	// 실행위치를 Root로 변경
	chdir("/");

	// 새로운 세션 부여
	setsid();
 
	int cnt = 0;
 
	// 반복 실행이 필요한 코드면 while문 안에 삽입
	while(1) {
		// daemon이 죽었을 경우 다시 실행
		if((pid = fork()) < 0) {
			break;
		}else if(pid == 0) {
			break;
		}else if(pid > 0) {
			wait(&status);
		}

		system("notify-send \'Angel\' \'척추 펴라\'");
		sleep(3600);
	}

	return 0;
}
