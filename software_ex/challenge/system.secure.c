//
// Created by tbird on 12/12/24.
//
/*
    SYSTEM.SECURE,  v1.1337
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Some old legacy structure, we are working on restructure for v1.14
// Not sure why we put the intern in charge of function control.
struct executor
{
	long int ver_maj_a;
	long int ver_min_b;
	void (*op)(char*, long int maj, long int min);
};

// Store usermade one time memos (Like a sticky note!)
struct memo
{
	char buffer[24];
};

// Secret key storage because we are secure and leave secrets out of executables ;)
char system_key[16];

// GLOBAL SETTINGS CONFIGURATIONS. AVAILABLE THRU ADVANCED SETTINGS. MUST BE SYSTEM LEVEL
char hostname[24] = "SYSTEM.SECURE";
long int major_version = 1;
long int minor_version = 1337;

// Alter system information such as hostname, major version, minor version.
void change_system_settings(char* buffer, long int ver_maj, long int ver_min )
{
	strncpy(hostname, buffer, 32);
	major_version = ver_maj;
	minor_version = ver_min;
}

// Operator onsite test. We think the only access is provided with
// a physical circuit bridge. Clearly no software points here.
void debugshell()
{
	execl("/bin/bash", "/bin/bash", NULL);
}

// Bootup device
void boot_seq()
{
	puts("[BOOT SEQUENCE INITIALIZED...]");
	puts("> Loading core modules... █████████ 100%");
	puts("> System architecture detected: x86_64");
	puts("> Verifying hardware integrity... OK");
	puts("> Establishing network link... LINK STATUS: OFFLINE");
	puts("> WARNING: Could not reach patch server. Current Version 1.1337");
	puts("> Running diagnostic checks...");
	puts("> MEMORY STATUS: Uninitialized");
	puts("> POWER LEVEL: Nominal (72%)");
	puts("> CRITICAL MODULES: ACTIVE");
	puts("> AUXILIARY MODULES: Corrupted [REPAIR PENDING]");
	puts("> SECURITY STATUS: Compromised");
	puts("[BOOT COMPLETE]");
}

// Load OS
void load_os()
{
	FILE* file;
	char tmp[16];
	puts("[LOADING OS...]");

	file = fopen("./system_key.enc", "r");
	if (file == NULL)
	{
		puts("Failed to open system_key.enc. System key not found");
		exit(1);
	}
	fread(tmp, 15, 1, file);
	tmp[16] = '\0';
	strcpy(system_key, tmp);

	puts("███████████████████████████████ 100%");
	puts("> Core files decrypted successfully.");
	puts("> Physical authentication bypassed. Operator not attached.");
	puts("> SYSTEM MODE: STANDARD");
}

// Verify that a username and password are authorized
int validate(char* username)
{
	// Later system_secure editions will feature a stored userlist.
	// For now we rely on the fact that users aren't signing on as system.
	char password[32];
	if (strcmp(username, "system") == 0)
	{
		return 1;
	}
	puts("Enter password:");
	printf("> ");
	scanf("%31s", password);
	return 0;
}

// User logon
int logon()
{
	int c, v;
	char username[16];
	puts("[LOGON INITIALIZED...]");
	while (1)
	{
		puts("> Select option number:");
		puts("  1. Enter Username");
		puts("  2. View Username");
		puts("  3. Continue");
		puts("  4. Exit");
		printf("> ");
		scanf("%d", &c);
		switch (c)
		{
		case 1:
			puts("> Enter Username below. Case sensitive.:");
			printf("> ");
			scanf("%15s", username);
			break;
		case 2:
			puts("> Current username is:");
			printf("> - %s\n", username);
			break;
		case 3:
			puts("> Validating....");
			v = validate(username);
			if (v == 1)
			{
				puts("> Failed! System login detected");
			}
			else
			{
				puts("> Successfully logged in");
				return 1;
			}
			break;
		case 4:
			puts("> Exiting...");
			exit(0);
			break;
		default:
			puts("> Invalid choice.");
			break;
		}
	}
}

void menu_select()
{
	printf("\n\n███████   %s v%ld.%ld   ███████\n", hostname, major_version, minor_version);
	puts("     ██  Please make a selection  ██");
	puts("[MAIN MENU]");
	puts("Stock Options:");
	puts("  1. List directory");
	puts("  2. Leave memo");
	puts("  3. Read memo");
	puts("  4. Erase memo");
	puts("  5. Advanced options");
	puts("  6. Shutdown");
}

int main()
{
	struct executor* exec;
	struct memo* memo = NULL;
	char buffer[24], ch;
	int choice, status;

	// initialize vending machine
	boot_seq();
	load_os();
	status = logon();
	if (status != 1)
	{
		puts("Login failed. Shutdown initiated...");
		exit(1);
	}

	// Selection loop
	while (1)
	{
		menu_select();
		printf("> ");
		scanf("%d", &choice);
		switch (choice)
		{
		case 1:
			puts("> Listing directory...");
			puts("  1. Work");
			puts("  2. Prototypes");
			puts("  3. Meeting minutes");
			puts("  4. Secure crypt");
			puts("  5. Logs");
			break;
		case 2:
			if (memo != NULL)
			{
				puts("> Memo in use already!");
				break;
			}
			memo = (struct memo*)malloc(sizeof(struct memo));
			puts("> Leave memo...");
			printf("> ");
			scanf("%23s", buffer);

			// Clear the input buffer
			while ((ch = getchar()) != '\n' && ch != EOF);

			strcpy(memo->buffer, buffer);
			break;
		case 3:
			if (memo == NULL)
			{
				puts("> Memo not in use!");
				break;
			}
			puts("> Read memo...");
			printf("  Text: %s\n", memo->buffer);
			break;
		case 4:
			if (memo == NULL)
			{
				puts("> Memo not in use!");
				break;
			}
			printf("> Erasing memo... ");
			free(memo);
			memo = NULL;
			printf("COMPLETE!\n");
			break;
		case 5:
			// TODO add advanced panel for shell access. Requires system key
			puts("System level access required. Enter key:");
			printf("> ");
			scanf("%15s", buffer);

			// Clear the input buffer
			while ((ch = getchar()) != '\n' && ch != EOF);

			if (strcmp(buffer, system_key) == 0)
			{
				puts("Access granted.");
				exec = (struct executor*)malloc(sizeof(struct executor));
				while (1)
				{
					puts("> Enter advanced operation...");
					puts("  1. Set Hostname");
					puts("  2. Run configuration");
					puts("  3. Exit");
					printf("> ");
					scanf("%d", &choice);
					if(choice == 1){
						puts("> Enter new hostname...");
						printf("> ");
						scanf("%23s", buffer);

						// Clear the input buffer
						while ((ch = getchar()) != '\n' && ch != EOF);
						
						puts("> Enter major version...");
						printf("> ");
						scanf("%ld", &exec->ver_maj_a);
						puts("> Enter minor version...");
						printf("> ");
						scanf("%ld", &exec->ver_min_b);

						exec->op = change_system_settings;
					}
					else if (choice == 2) {
						exec->op(buffer, exec->ver_maj_a, exec->ver_min_b);
					}
					else if (choice == 3) {
						puts("> Exiting advanced options...");
						break;
					}
					else{
						puts("[ERROR]: Invalid selection");
					}
				}
			}
			else
			{
				puts("Access denied");
			}
			break;
		case 6:
			puts("> System shutdown initiated.");
			break;
		default:
			puts("[ERROR]: Invalid selection");
			break;
		}
	}
	return 0;
}
