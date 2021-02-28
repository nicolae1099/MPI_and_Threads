#include "mpi.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <ctype.h>
#include <bits/stdc++.h> 
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <algorithm>

#define MASTER 0
#define HORROR 1
#define COMEDY 2
#define FANTASY 3
#define SF 4

using namespace std;

int number_of_threads = 4;
string inputFilename;
string outputFilename;
pthread_barrier_t barrier;
//vector<string> senderBuffer;
//int horrorIterator;
int maxThreads = sysconf(_SC_NPROCESSORS_CONF);
map<int, string> mapa;
//map<int, string> horrorMap;

/** 
 * efectuez citirea fisierul de intrare in paralel. 
 * Fiecare thread citeste line by line si atunci cand da peste genul corespunzator,
 * va trimite workerului corespunzator acel paragraf.
 * va astepta de la worker varianta procesata si o va insera intr-un map pt afisarea 
 * in ordinea corecta in procesul MASTER
 */
void *thread_function(void *arg) {
    string line;
    string result;
    string horrorLines;
    string comedyLines;
    string fantasyLines;
    string sfLines;
    MPI_Status status;
    string s;
    int count;
    int counter = -1;
    string stopSemafor = "stopSemafor";

	int thread_id = *(int *)arg;
    ifstream fin;
    fin.open(inputFilename);

    while (getline(fin, line)) {
        if (line == "horror" || line == "comedy" || line == "fantasy" || line == "science-fiction") {
            counter++;
        }
        if ((line == "horror") && (thread_id == HORROR - 1)) {
            while (getline(fin, line)) {
                if (line.size() == 0) {
                    horrorLines.append(" \n");
                    MPI_Send(&horrorLines[0], horrorLines.size()+1, MPI_CHAR, HORROR, 123, MPI_COMM_WORLD);
                    horrorLines.erase();
                    MPI_Probe(HORROR, 123, MPI_COMM_WORLD, &status);
                    MPI_Get_count(&status, MPI_CHAR, &count);
                    char buf1 [count];
                    MPI_Recv(&buf1, count, MPI_CHAR, HORROR, 123, MPI_COMM_WORLD, &status);
                    s = buf1;
                    mapa.insert({counter, s});
                    break;
                }
                horrorLines.append(line).append(" \n");
            }
        } else if ((line == "comedy") && (thread_id == COMEDY - 1)) {
            while (getline(fin, line)) {
                if (line.size() == 0) {
                    comedyLines.append(" \n");
                    MPI_Send(&comedyLines[0],comedyLines.size()+1,MPI_CHAR, COMEDY,123,MPI_COMM_WORLD);
                    comedyLines.erase();

                    MPI_Probe(COMEDY,123,MPI_COMM_WORLD,&status);
                    MPI_Get_count(&status,MPI_CHAR,&count);
                    char buf2 [count];
                    MPI_Recv(&buf2,count,MPI_CHAR,COMEDY,123,MPI_COMM_WORLD,&status);
                    s = buf2;
                    mapa.insert({counter, s});
                    break;
                } else {
                    comedyLines.append(line).append(" \n");
                }
            }
        } else if ((line == "fantasy") && (thread_id == FANTASY - 1)) {
            while (getline(fin, line)) {                
                if (line.size() == 0) {
                    fantasyLines.append(" \n");
                    MPI_Send(&fantasyLines[0], fantasyLines.size()+1,MPI_CHAR,FANTASY,123,MPI_COMM_WORLD);
                    fantasyLines.erase();

                    MPI_Probe(FANTASY,123,MPI_COMM_WORLD,&status);
                    MPI_Get_count(&status,MPI_CHAR,&count);
                    char buf3 [count];
                    MPI_Recv(&buf3,count,MPI_CHAR,FANTASY,123,MPI_COMM_WORLD,&status);
                    s = buf3;
                    mapa.insert({counter, s});
                    break;
                } else {
                    fantasyLines.append(line).append(" \n");
                }
            }
        } else if ((line == "science-fiction") && (thread_id == SF - 1)) {
            while (getline(fin, line)) {
                if (line.size() == 0) {
                    sfLines.append(" \n-");
                    MPI_Send(&sfLines[0],sfLines.size()+1,MPI_CHAR,SF,123,MPI_COMM_WORLD);
                    sfLines.erase();
                    
                    MPI_Probe(SF,123,MPI_COMM_WORLD,&status);
                    MPI_Get_count(&status,MPI_CHAR,&count);
                    char buf4 [count];
                    MPI_Recv(&buf4,count,MPI_CHAR,SF,123,MPI_COMM_WORLD,&status);
                    s = buf4;
                    mapa.insert({counter, s});
                    break;
                } else {
                    sfLines.append(line).append(" \n-");
                }
            }
        }
    }
    pthread_barrier_wait(&barrier);
    horrorLines.append("stopSemafor");
    MPI_Send(&horrorLines[0], horrorLines.size() + 1, MPI_CHAR, HORROR, 123 ,MPI_COMM_WORLD);
    MPI_Send(&horrorLines[0], horrorLines.size() + 1, MPI_CHAR, COMEDY, 123 ,MPI_COMM_WORLD);
    MPI_Send(&horrorLines[0], horrorLines.size() + 1, MPI_CHAR, FANTASY, 123 ,MPI_COMM_WORLD);
    MPI_Send(&horrorLines[0], horrorLines.size() + 1, MPI_CHAR, SF, 123 ,MPI_COMM_WORLD);

    fin.close();
	pthread_exit(NULL);	
}

void *compute_horror_thread(void *arg) {
    int thread_id = *(int *)arg;
    //string s = senderBuffer[thread_id];
    string s;
    string result;
    for (int i = 0; i < s.length(); i++) {
        if (!isspace(s[i]) && s[i] != 'a' && s[i] != 'e' && s[i] != 'i' && s[i] != 'o' && s[i] != 'u'
            && s[i] != 'A' && s[i] != 'E' && s[i] != 'I' && s[i] != 'O' && s[i] != 'U' && s[i] != '.') {
            result += s[i];
            result += tolower(s[i]);
        } else {
            result += s[i];
        }
    }
  //  horrorMap.insert({thread_id, result});
    pthread_exit(NULL);	
}

int main (int argc, char *argv[])
{
    pthread_t tid[number_of_threads];
	int thread_id[number_of_threads];

    int provided;
    int  numtasks, rank;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int count;
    MPI_Status status;
    string s;
    inputFilename = argv[1];
    outputFilename = inputFilename;
    pthread_barrier_init(&barrier, NULL, number_of_threads);
    ofstream fout;
    fout.open (outputFilename.erase(18,3).append("out"));

    if (rank == MASTER) {
        for (int i = 0; i < number_of_threads; i++) {
		    thread_id[i] = i;
		    pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	    }

        for (int i = 0; i < number_of_threads; i++) {
            pthread_join(tid[i], NULL);
        }
        pthread_barrier_destroy(&barrier);

        map<int,string>::iterator it = mapa.begin();
        for (it=mapa.begin(); it!= mapa.end(); ++it) {
            fout << it->second;
        }
        fout.close();

    } else if (rank == HORROR) {
        while (true) {

            MPI_Probe(0,123,MPI_COMM_WORLD,&status);
            MPI_Get_count(&status,MPI_CHAR,&count);
            char buf [count];
            MPI_Recv(&buf,count,MPI_CHAR,0,123,MPI_COMM_WORLD,&status);
            s = buf;
            if (s == "stopSemafor") {
                break;
            }

            string result = "horror";
            result += "\n";
            for (int i = 0; i < s.length(); i++) {
                if (!isspace(s[i]) && s[i] != 'a' && s[i] != 'e' && s[i] != 'i' && s[i] != 'o' && s[i] != 'u'
                    && s[i] != 'A' && s[i] != 'E' && s[i] != 'I' && s[i] != 'O' && s[i] != 'U' && s[i] != '.') {
                    result += s[i];
                    result += tolower(s[i]);
                } else {
                    result += s[i];
                }
            }
            MPI_Send(&result[0],result.size()+1,MPI_CHAR,0,123,MPI_COMM_WORLD);         
            /*
            int countLines = 1;
            vector<int> positions;
            positions.push_back(0);
            for (int i = 0; i < s.length(); i++) {
                if (s[i] == '-') {
                    countLines++;
                    s.erase(s.begin() + i, s.begin() + i + 1);
                }
                if (countLines == 20) {
                    countLines = 0;
                    positions.push_back(i);
                }
            }
            if (countLines != 0) {
                positions.push_back(s.length());
            }

            horrorIterator = 1;
            int x = (positions.size() - 1) % (maxThreads - 1);
            int num_threads = max(2, x);
            
            senderBuffer.resize(positions.size());
            /*
            for (int i = 0 ; i < positions.size(); i++) {
                cout << positions[i] << endl;
            }
    
                pthread_t horrorThreads[num_threads];
	            int horrorThreadId[num_threads];
           // cout << positions.size() << " " << num_threads << endl;
            while (horrorIterator < positions.size() - num_threads) {

                for (int i = 0; i < num_threads; i++) {
                    senderBuffer[horrorIterator] = s.substr(positions[horrorIterator - 1] , positions[horrorIterator] - positions[horrorIterator-1]);
                    //senderBuffer = ;
                    horrorThreadId[i] = horrorIterator;
                    horrorIterator++;               
                    pthread_create(&horrorThreads[i], NULL, compute_horror_thread, &horrorThreadId[i]);
                    
                }
                
                // se asteapta thread-urile
                for (int i = 0; i < num_threads; i++) {
                    pthread_join(horrorThreads[i], NULL);
                }
            }

            int num = positions.size() - horrorIterator;
            pthread_t horrorThreads2[num];
	        int horrorThreadId2[num];
            for (int i = 0; i < num; i++) {
                senderBuffer[horrorIterator] = s.substr(positions[horrorIterator - 1] , positions[horrorIterator] - positions[horrorIterator-1]);
                //senderBuffer = ;
                horrorThreadId2[i] = horrorIterator;
                horrorIterator++;              
                pthread_create(&horrorThreads2[i], NULL, compute_horror_thread, &horrorThreadId2[i]);
                
            }
            
            // se asteapta thread-urile
            for (int i = 0; i < num; i++) {
                pthread_join(horrorThreads2[i], NULL);
            }

            //int num_threads = 
            //senderBuffer.resize(0);
            string result = "horror";
            result += "\n";
            map<int,string>::iterator it = horrorMap.begin();
            for (it=horrorMap.begin(); it!= horrorMap.end(); ++it) {
                result.append(it->second);
            }
            horrorMap.clear();
            positions.resize(0);
            senderBuffer.resize(0);
            horrorIterator = 1;
            */

            
        }
        
    } else if (rank == COMEDY) {
        while (true) {
            MPI_Probe(0,123,MPI_COMM_WORLD,&status);
            MPI_Get_count(&status,MPI_CHAR,&count);
            char buf [count];
            MPI_Recv(&buf,count,MPI_CHAR,0,123,MPI_COMM_WORLD,&status);
            s = buf;
            if (s == "stopSemafor") {
                break;
            }
            string result = "comedy";
            result += "\n";
            int lettersCount = 0;
            for (int i = 0; i < s.length(); i++) {
                if (isspace(s[i])) {
                    lettersCount = 0;
                } else {
                    lettersCount++;
                }
                if (lettersCount % 2 == 0) {
                    result += toupper(s[i]);
                } else {
                    result += s[i];
                }
            }

            MPI_Send(&result[0],result.size()+1,MPI_CHAR,0,123,MPI_COMM_WORLD); 
        }

    } else if (rank == FANTASY) {
        while (true) {
            MPI_Probe(0,123,MPI_COMM_WORLD,&status);
            MPI_Get_count(&status,MPI_CHAR,&count);
            char buf [count];
            MPI_Recv(&buf,count,MPI_CHAR,0,123,MPI_COMM_WORLD,&status);
            s = buf;
            if (s == "stopSemafor") {
                break;
            }
            string result = "fantasy";
            result += "\n";
            result += toupper(s[0]);
            for (int i = 1; i < s.length(); i++) {
                if (isspace(s[i-1])) {
                    result += toupper(s[i]);
                } else {
                    result += s[i];
                }
            }

            MPI_Send(&result[0],result.size()+1,MPI_CHAR,0,123,MPI_COMM_WORLD); 
        }
        
     } else if (rank == SF) {
        while (true) {
            MPI_Probe(0,123,MPI_COMM_WORLD,&status);
            MPI_Get_count(&status,MPI_CHAR,&count);
            char buf [count];
            MPI_Recv(&buf,count,MPI_CHAR,0,123,MPI_COMM_WORLD,&status);
            s = buf;
            if (s == "stopSemafor") {
                break;
            }
            int wordCount = 0;
            int left = 0;
            int right = 0;
            string result = "science-fiction";
            result += "\n";

            for (int i = 0; i < s.length(); i++) {
                if (s[i] == '-') {
                    s.erase(s.begin() + i, s.begin() + i + 1);
                    wordCount = 0;
                }
                if (isspace(s[i])) {
                    wordCount++;
                }
                if (wordCount == 6) {
                    left = i+1;
                    wordCount++;
                }
                if (wordCount == 8) {
                    right = i;
                    wordCount = 0;
                    reverse(s.begin() + left, s.begin() + right);
                }
            }
            result.append(s);
            MPI_Send(&result[0], result.size()+1, MPI_CHAR, 0, 123, MPI_COMM_WORLD); 
        }
        
     }
    MPI_Finalize();
}


