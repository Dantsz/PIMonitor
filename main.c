
#include <gtk-2.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#define BUFFERSIZE 34

static double avg = 0.0f;
static double reads  = 0.0f;

void end_program (GtkWidget *wid, gpointer ptr)
    {
    gtk_main_quit ();
    }

    //puts the dot in the temperature string
void dotString(char* str)
    {   
        //make the free buffer data null  
        int length = strlen(str);
        if(length > BUFFERSIZE || length < 4) return;
        for(int i = length ; i < BUFFERSIZE ; i ++) str[i] = '\0';
        //move the 3 decimals one position ahead
        
        for(int i = length ; i > length - 4; i --)
        {
            str[i] = str[i - 1];
        }
        str[length - 4] = '.';
        str[length + 1] = '\0';
    }

//reads the temperature into the string
void readTemperature(char** string, size_t* len)
{
    FILE* tempFILE = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    if(tempFILE == NULL)
    {
            printf("COULD NOT OPEN TEMPERATURE FILE\n");
    }
        
    getline(string,len,tempFILE);
    fclose(tempFILE);
}

int update(unsigned char ** data)
    {
        ///
        unsigned long* temps = (unsigned long*)data[0];
        char* curTempString = (char*) data[1];
        char* minTempString = (char*) data[2];
        char* maxTempString = (char*) data[3];
        size_t len = BUFFERSIZE;
        readTemperature(&curTempString,&len);
        sscanf(curTempString,"%lu",&temps[0]);
        
        //check min temp
        if(temps[0] < temps[1])
        {
            strncpy(minTempString,curTempString,BUFFERSIZE);
            temps[1] = temps[0];
            dotString(minTempString);
            gtk_label_set_text((GtkLabel*)data[5] , minTempString);
        }
        //check max temp
        if(temps[0] > temps[2])
        {
            strncpy(maxTempString,curTempString,BUFFERSIZE);
            temps[2] = temps[0];
            dotString(maxTempString);
            gtk_label_set_text((GtkLabel*)data[6] , maxTempString);
        }

  
        //current
        dotString(curTempString);
        gtk_label_set_text((GtkLabel*)data[4],curTempString);

        //average 
        double curTDouble = atof(curTempString);
        avg *= reads;
        reads += 1.0f;
        avg += curTDouble;
        avg /= reads;
        snprintf(data[7], BUFFERSIZE,"%.3lf",avg);
        gtk_label_set_text((GtkLabel*)data[8],data[7]);
        



        return 1;

    }

int main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    GtkWidget* win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (win, "delete_event", G_CALLBACK (end_program),NULL);
    gtk_window_set_default_size(win , 300 ,50 );
    GtkWidget* table = gtk_table_new(4,6,0);



    //temperature widgets
    GtkWidget* curtemp = gtk_label_new("0.0");
    gtk_table_attach_defaults(GTK_TABLE(table),curtemp,1,2,1,2);
    GtkWidget* mintemp = gtk_label_new("0.0");
    gtk_table_attach_defaults(GTK_TABLE(table),mintemp,2,3,1,2);
    GtkWidget* maxtemp = gtk_label_new("0.0");
    gtk_table_attach_defaults(GTK_TABLE(table),maxtemp,3,4,1,2);
    GtkWidget* avgtemp = gtk_label_new("0.0");
    gtk_table_attach_defaults(GTK_TABLE(table),avgtemp,4,5,1,2);
    //First Row
    GtkWidget* curText = gtk_label_new(" Current ");
    gtk_table_attach_defaults(GTK_TABLE(table),curText,1,2,0,1);
    GtkWidget* minText = gtk_label_new("    Min   ");
    gtk_table_attach_defaults(GTK_TABLE(table),minText,2,3,0,1);
    GtkWidget* maxText = gtk_label_new("    Max   ");
    gtk_table_attach_defaults(GTK_TABLE(table),maxText,3,4,0,1);
    GtkWidget* avgText = gtk_label_new("    AVG   ");
    gtk_table_attach_defaults(GTK_TABLE(table),avgText,4,5,0,1);
    //
    //Temp text
    GtkWidget* tempText = gtk_label_new("Temp °C: ");
    gtk_table_attach_defaults(GTK_TABLE(table),tempText,0,1,1,2);


    

    gtk_container_add(GTK_CONTAINER(win),table);


    unsigned long tempsInNumbers[3] = {0,ULONG_MAX,0};
    char* curTempString = malloc(BUFFERSIZE * sizeof(char));
    char* minTempString = malloc(BUFFERSIZE* sizeof(char));
    char* maxTempString = malloc(BUFFERSIZE * sizeof(char));
    char* avgTempString = malloc(BUFFERSIZE* sizeof(char));
    
    char** data[9] = {tempsInNumbers,curTempString,minTempString,maxTempString,curtemp,mintemp,maxtemp,avgTempString,avgtemp};
    
    g_timeout_add_seconds(1.0, update, data);
    gtk_widget_show_all (win);

   

    gtk_main ();
    
    free(curTempString);
    free(minTempString);
    free(maxTempString);
    free(avgTempString);
    return 0;
}