#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <wait.h>

struct Passenger {
    char vname[30];
    char kname[30];
    char phone[30];
    char place[30];
    char mode[30];      
};

struct msg_t
{
	long mtype;
	char mtext[1024];
};

sem_t* semid;

void getName(char name[30])
{
    scanf(" %30[^\n]s", name);
    
    while (getchar() != '\n');

    if(strlen(name) == 30)
    {
        name[29] = '\0'; 
    }
}

void getPlace(char place[30])
{
    int correct = 1;
    char number;
    char correctChar;
    int numberCounter;
    while(correct)
    {
        numberCounter = -1;
        correctChar = 'x';

        printf("Helyszín kiválasztása:\n 1. Bali \n 2. Mali \n 3. Cook szigetek \n 4. Bahamák \n 5. Izland \n");
        scanf(" %c", &number);

        while(correctChar != '\n')
        {
            scanf("%c", &correctChar);
            numberCounter = numberCounter + 1;
        }
        if(numberCounter != 0){ number = '6'; } 
        
        correct = 0;

        switch (number)
        {
        case '1':
            strcpy(place,"Bali");
            break;
        case '2':
            strcpy(place,"Mali");
            break;
        case '3':
            strcpy(place,"Cook szigetek");
            break;
        case '4':
            strcpy(place,"Bahamák");
            break;
        case '5':
            strcpy(place,"Izland");
            break;
        default:
            correct = 1;
            printf("Ilyen sorszámú helyszín nem szerepel a listában! Próbálja újra! \n");
            break;
        }    
    }

}

void getPhone(char phone[30])
{
    int correct = 1;
    while(correct)
    {
        printf("Telefonszám:\n");
        scanf(" %30[^\n]s", phone);

        while (getchar() != '\n');

        if(strlen(phone) == 30)
        {
            phone[29] = '\0'; 
        }

        int count = 0;
        int length = strlen(phone);
        int i;
        for(i = 0; i < length; i++)
        {
            if(isdigit(phone[i]))
            { 
                count = count + 1; 
            }
        }
        if(count == length)
        { 
            correct = 0; 
        }
        else
        {
            printf("A telefonszámnak minden karaktere szám kell hogy legyen\n");
        }        
    }
}

void getMode(char mode[30])
{
    int correct = 1;
    char number;
    int numberCounter;
    char correctChar;
    while(correct)
    {
        numberCounter = -1;
        correctChar = 'x';
        
        printf("Utazási mód:\n 1. repülő \n 2. autóbusz \n 3. hajó \n");
        scanf(" %c", &number);

        while(correctChar != '\n')
        {
            scanf("%c", &correctChar);
            numberCounter = numberCounter + 1;
        }
        if(numberCounter != 0){number = '6';} 

        correct = 0;
        switch (number)
        {
        case '1':
            strcpy(mode,"repülő");
            break;
        case '2':
            strcpy(mode,"autóbusz");
            break;
        case '3':
            strcpy(mode,"hajó");
            break;
        default:
            correct = 1;
            printf("Ilyen sorszámú utazási mód nem szerepel a listában! Próbálja újra! \n");
            break;
        }
    }
}

void NewStruct()
{
    struct Passenger newPassenger;

    printf("Új utas adatainak felvétele.\n");

	printf("Vezetéknév:\n");
    getName(newPassenger.vname);

    printf("Keresztnév:\n");
    getName(newPassenger.kname);

    getPhone(newPassenger.phone);
    getPlace(newPassenger.place);
    getMode(newPassenger.mode);

    sem_wait(semid);

    int f = open("listing.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (f < 0)
    {
        perror("File opening error\n"); 
        exit(1);
    }
    
    if( write(f, &newPassenger, sizeof(newPassenger)) != sizeof(newPassenger) )
    {
        perror("Hiba történt a fálj írásakor.\n");
        exit(1);
    }
    close(f);

    sem_post(semid); 
}

void EditStruct()
{
    char telephone[30];
    printf("Kérlek add meg a módosítandó utas telefonszámát: \n");
    getName(telephone);

    int found = 1;
    struct Passenger pass;
    char editedData[30];

    int correct = 1;
    char editNumber;
    char correctEdit;
    int editNumberCounting;
    while(correct)
    {
        editNumberCounting = -1;
        correctEdit = 'x';
        
        printf("Válassza ki, hogy melyik adatot szeretné módosítani!\n 1.Vezetéknév \n 2.Keresztnév \n 3.Telefonszám \n 4.Helyszín \n 5.Utazási mód\n");
        scanf(" %c", &editNumber);

        while(correctEdit != '\n')
        {
            scanf("%c", &correctEdit);
            editNumberCounting = editNumberCounting + 1;
        }
        if(editNumberCounting != 0){editNumber = '6';} 

        correct = 0;
        switch (editNumber)
        {
        case '1':
            printf("Vezetéknév:\n");
            getName(editedData);
            break;
        case '2':
            printf("Keresztnév:\n");
            getName(editedData);
            break;
        case '3':
            getPhone(editedData);
            break;
        case '4':
            getPlace(editedData);
            break;
        case '5':
            getMode(editedData);
            break;
        default:
            correct = 1;
            printf("Ilyen sorszámú adat nem szerepel a listában! Próbálja újra! \n");
            break;
        }
    }
    sem_wait(semid);
    int f = open("listing.txt", O_RDONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (f < 0)
    {
        perror("Hiba a fájl megnyitásakor\n"); 
        exit(1);
    }
    int g = open("helper.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (g < 0)
    {
        perror("Hiba a fájl megnyitásakor\n"); 
        exit(1);
    }
    while(read(f, &pass, sizeof(pass)))
    {
        if (strcmp(telephone, pass.phone) == 0)
        {
            found = 0;
            switch (editNumber)
            {
            case '1':
                strcpy(pass.vname, editedData);
                break;
            case '2':
                strcpy(pass.kname, editedData);
                break;
            case '3':
                strcpy(pass.phone, editedData);
                break;
            case '4':
                strcpy(pass.place, editedData);
                break;
            case '5':
                strcpy(pass.mode, editedData);
                break;
            }
        }                    
        if(write(g, &pass, sizeof(pass)) != sizeof(pass))
        {
            perror("Hiba történt a fálj írásakor.\n");
            exit(1);
        }
    } 
    close(f);
    close(g);
    remove("listing.txt");
    rename("helper.txt", "listing.txt");
    sem_post(semid); 

    if (found == 1) 
    {
        printf("Nincs ilyen telefonszámú utas!\n");
    }
}

void DeleteStruct()
{
    char telephone[30];
    printf("Kérlek add meg a módosítandó utas telefonszámát: \n");
    getName(telephone);

    int found = 1;    
    struct Passenger pass;

    sem_wait(semid);
    int f = open("listing.txt", O_RDONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (f < 0)
    {
        perror("Hiba a fájl megnyitásakor.\n"); 
        exit(1);
    }
    int g = open("helper.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (g < 0)
    {
        perror("Hiba a fájl megnyitásakor.\n"); 
        exit(1);
    }

    while(read(f, &pass, sizeof(pass)))
    {
        if (strcmp(telephone, pass.phone) == 0)
        {
            found = 0;
        } 
        else 
        {
            if(write(g, &pass, sizeof(pass)) != sizeof(pass))
            {
                perror("Hiba történt a fájl írásakor.\n");
                exit(1);
            }
        }         
    } 

    if (found == 1) 
    {
        printf("Nincs ilyen telefonszámú utas!\n");
    }
    close(f);
    close(g);
    remove("listing.txt");
    rename("helper.txt", "listing.txt");
    sem_post(semid); 
}

void ListStruct()
{    
    char theplace[20];
    getPlace(theplace);

    sem_wait(semid);
    int f = open("listing.txt", O_RDONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (f < 0)
    {
        perror("Hiba a fájl megnyitásakor.\n"); 
        exit(1);
    }

    struct Passenger pass;

    printf("%s-n ragadt utasok listája: \n", theplace);
    
    while(read(f, &pass, sizeof(pass)))
    {
        if (strcmp(theplace, pass.place) == 0)
        {
            printf("%s %s \t %s \t %s \t %s \n", pass.vname, pass.kname, pass.phone, pass.place, pass.mode);
        }            
    }
    close(f);
    sem_post(semid); 
}

int isThereEnoughPassenger(int n)
{
    int i;
    int result = 0;

    sem_wait(semid);
    int f = open("listing.txt", O_RDONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (f < 0)
    {
        perror("Hiba a fájl megnyitásakor\n"); 
        exit(1);
    }
    struct Passenger pass;
    while(read(f, &pass, sizeof(pass)))
    {
        if(n == 0 && strcmp(pass.place, "Bali") == 0){ result++; }
        else if(n == 1 && strcmp(pass.place, "Mali") == 0){ result++; }
        else if(n == 2 && strcmp(pass.place, "Cook szigetek") == 0){ result++; }
        else if(n == 4 && strcmp(pass.place, "Bahamák") == 0){ result++; }
        else if(n == 5 && strcmp(pass.place, "Izland") == 0){ result++; }
    }

    close(f);
    sem_post(semid);
    if(result >= 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

void handler1(int signum, siginfo_t *info, void *context)
{
	psignal(signum, "\nCsődbiztos: Értettem, küldöm a listát.\n");
    char place[30];

    int svPlace = info->si_value.sival_int;

    switch (svPlace)
        {
        case 0:
            strcpy(place,"Bali");
            break;
        case 1:
            strcpy(place,"Mali");
            break;
        case 2:
            strcpy(place,"Cook szigetek");
            break;
        case 3:
            strcpy(place,"Bahamák");
            break;
        case 4:
            strcpy(place,"Izland");
            break;
        }
    struct Passenger pass;

    char pipename[20];
    sprintf(pipename,"/tmp/%d",getppid());

    sem_wait(semid);

    int g = open(pipename, O_WRONLY);
    int f = open("listing.txt", O_RDONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    int h = open("help.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    

    while(read(f, &pass, sizeof(pass)))
    {
        if (strcmp(place, pass.place) == 0)
        {
            if(write(g, &pass, sizeof(pass)) != sizeof(pass))
            {
                perror("Hiba történt a fálj írásakor. g\n");
                exit(1);
            }
        }   
        else
        {
            if(write(h, &pass, sizeof(pass)) != sizeof(pass))
            {
                perror("Hiba történt a fájl írásakor. h\n");
                exit(1);
            }
        }
                            
    }
    close(g);
    close(f);
    close(h);
    remove("listing.txt");
    rename("help.txt", "listing.txt");
    sem_post(semid);
}

void handler2(int signum, siginfo_t *info, void *context)
{
    struct msg_t myRcvdMsg;

    int mq_id = info->si_value.sival_int;

    sem_wait(semid);
    int status = msgrcv(mq_id, &myRcvdMsg, 1024,  5, 0);
    sem_post(semid);

    if(status < 0){perror("msgrcv");}

    printf("\nCsődbiztos: a mentőexpedíciótól kapott üzenet:\n");
    printf("%s\n", myRcvdMsg.mtext);
	psignal(signum, "Csődbiztos: Értettem, a hazaérkezetteket töröltük a listáról.\n");
}

void hand(int signum)
{
    printf("Csődbiztos: Lista ellenőrzése.\n");
}

int main(int argc, char* argv[])
{
    int exitNumber = 1;
    char menuNumber;
    char correctMenu;
    int menuNumberCounting;

    int mq_id;
    int savePlace;
    int precedePassenger[5] = {0, 0, 0, 0, 0};
    int enoughPassenger[5];

    struct sigaction action1;
	struct sigaction action2;
	action1.sa_sigaction = &handler1;
	action2.sa_sigaction = &handler2;
	action1.sa_flags = SA_SIGINFO;
	action2.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &action1, NULL);
	sigaction(SIGUSR2, &action2, NULL);

    signal(SIGINT, hand);

    char pipename[20];
    sprintf(pipename,"/tmp/%d",getpid());
    int fid = mkfifo(pipename, S_IRUSR|S_IWUSR);
    if (fid == -1)
    {
        printf("Error number: %i",errno);
        perror("Gaz van:");
        exit(EXIT_FAILURE);
    }

    key_t key = ftok(argv[0], 0666);
    mq_id = msgget(key, 0666|IPC_CREAT);
    
    semid = sem_open("/semi",O_CREAT,S_IRUSR|S_IWUSR,0);
    sem_post(semid);

    pid_t csodb = fork();

    if(csodb > 0)
    {
        while(exitNumber)
        {
            
            menuNumberCounting = -1;
            correctMenu = 'x';
            
            printf("\n Kérem válasszon a listából a menüpont sorszámának megadásával!\n1. Új utas felvétele a listára.\n2. Már felvett utas adatainak módosítása.\n3. Felvett utas adatainak törlése.\n4. Utasok adatainak helyszín szerinti listázása.\n5. Kilépés.\n");
            
            scanf(" %c", &menuNumber);
                        
            while(correctMenu != '\n')
            {
                scanf("%c", &correctMenu);
                menuNumberCounting = menuNumberCounting + 1;
            }
            if(menuNumberCounting != 0){ menuNumber = '6'; }

            switch (menuNumber)
            {
            case '1':
                NewStruct();
                break;
            case '2':
                EditStruct();
                break;
            case '3':
                DeleteStruct();
                break;
            case '4':
                ListStruct();
                break;
            case '5':
                printf("Viszlát! \n");
                exitNumber = 0;                
                msgctl(mq_id, IPC_RMID, NULL);
                unlink(pipename);
                sem_unlink("/semi");
                kill(csodb,SIGKILL);
                break;
            default:
                printf("Ilyen sorszámú menüpont nem szerepel a listában! Próbálja újra! \n");
                break;
            }
            kill(csodb, SIGINT);
        }      
    }
    else if (csodb == 0)
    {
        while(1)
        {
            pid_t child = 1;
            int i;
            if (child > 0)
            {
                for(i = 0; i < 5; i++)
                {
                    enoughPassenger[i] = isThereEnoughPassenger(i);
                    if(enoughPassenger[i] == 0)
                    {
                        precedePassenger[i] = 0;
                    }            
                }            

                for(i = 0; i < 5; i++)
                {
                    if(precedePassenger[i] != enoughPassenger[i] && enoughPassenger[i] != 0)
                    {
                        precedePassenger[i] = enoughPassenger[i];
                        if(child != 0)
                        {            
                            printf("Csődbiztos: Mentőexpedíció indítása.\n");        
                            child = fork();
                        }
                        if(child == 0)
                        {
                            savePlace = i;
                            break;
                        }     
                    }
                }
            }
            if(child == 0)
            {   
                int sleepint = 2 + savePlace * 2;
                char place[20];
                switch (savePlace)
                {
                case 0:
                    strcpy(place,"Bali");
                    break;
                case 1:
                    strcpy(place,"Mali");
                    break;
                case 2:
                    strcpy(place,"Cook szigetek");
                    break;
                case 3:
                    strcpy(place,"Bahamák");
                    break;
                case 4:
                    strcpy(place,"Izland");
                    break;
                }

                struct Passenger pass;
                char svPl[50] = "Ennyit mentettünk: ";
                char strdb[12];
                

                printf("\nMentőexpedíció: Elindultunk %s-ra.\n", place);
                sleep(sleepint);
                printf("\nMentőexpedíció: Megérkeztünk %s-ra.\n", place);

                union sigval value1;
                value1.sival_int = savePlace;
                sigqueue(getppid(), SIGUSR1, value1);

                sleep(2);

                int db = 0;

                int g = open(pipename, O_RDONLY);
                printf("\nMentőexpedíció %s: Megkaptam a listát.\n", place);
                while(read(g, &pass, sizeof(pass)))
                {
                    db = db + 1;
                    printf("Mentőexpedíció %s: Olvasom: %s %s \t %s \t %s \t %s \n", place, pass.vname, pass.kname, pass.phone, pass.place, pass.mode);
                }
                close(g);
                sleep(sleepint);

                sprintf(strdb, "%d", db);
                strcat(svPl, strdb);
                strcat(svPl, ", innen: ");
                strcat(svPl, pass.place);

                struct msg_t mymsg;
                mymsg.mtype = 5;
                strcpy(mymsg.mtext, svPl);

                sem_wait(semid);
                int status = msgsnd(mq_id, &mymsg, strlen(mymsg.mtext)+1, 0);
                sem_post(semid);

                if(status < 0){perror("msgsnd");}
                
                union sigval value2;
                value2.sival_int = mq_id;
                sigqueue(getppid(), SIGUSR2, value2);

                break;
            }
            if(child > 0)
            {
                pause(); 
            }
                   
        }
    }
}