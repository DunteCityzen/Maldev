#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libssh/libssh.h>

int isValiduser(const char* username){
    if(strcmp(username, "c2user")){
        return 1;
    }
    return 0;
}

int main(){
    ssh_bind sshbind;
    ssh_session session;
    int rc;

    sshbind = ssh_bind_new();
    if(sshbind == NULL){
        printf("! Failed to create sshbind object\n! Exiting program...\n");
        exit(EXIT_FAILURE);
    }

    printf("* Setting the bind options\n");
    rc = ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, 2222);

    if(rc != SSH_OK){
        printf("! Failed to set options for ssh server due to error: %s\n", ssh_get_error(sshbind));
        ssh_bind_free(sshbind);
        exit(EXIT_FAILURE);
    }

    rc = ssh_bind_listen(sshbind);

    if(rc != SSH_OK){
        printf("! Server unable to listen on the specified port due to: %s\n", ssh_get_error(sshbind));
        ssh_bind_free(sshbind);
        exit(EXIT_FAILURE);
    }

    printf("* SSH server successfully listening on specified port\n");

    while(1){
        session = ssh_new();
        if(session == NULL){
            printf("! Error creating ssh session\n");
            continue;
        }

        rc = ssh_bind_accept(sshbind, session);

        if(rc != SSH_OK){
            printf("Error accepting connection due to: %s", ssh_get_error(session));
            ssh_free(session);
            continue;
        }

        printf("* Attempting Key exchange...\n");
        rc = ssh_key_exchange(session);

        if(rc != SSH_OK){
            printf("Key exchange failed due to error: %s\n", ssh_get_error(session));
            continue;
        }

        printf("* Key exchange successful\n");

        const char* username = ssh_get_issue_banner(session);

        if(username == NULL){
            printf("! Failed to get the username from the attempted session due to error: %s\n", ssh_get_error(session));
            ssh_disconnect(session);
            ssh_free(session);
            continue;
        }

        if(!(isValiduser(username))){
            printf("! Get out of here imposter\n");
            ssh_disconnect(session);
            ssh_free(session);
            continue;
        }

        rc = ssh_userauth_password(session, username, "Passwd1!");

        if(rc == SSH_AUTH_SUCCESS){
            printf("**** Welcome back, %s! *****\n", username);
        } else{
            printf("Authentication failed due to incorrect username/password\n");
        }

        ssh_disconnect(session);
        ssh_free(session);
    }

    ssh_bind_free(sshbind);

    return 0;
}