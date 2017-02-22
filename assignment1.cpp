// Chien Nguyen
// COSC 3360
// PS#1328764

#include <iostream>
#include <string>

using namespace std;

int currentTime = 0, count2 = -1;
int ncores = 0, ncores1 = 0, slice = 0;
int diskCount = 0, readyCount = 0;

struct data {
  string state;
  int place;
  string process[100];
  int timeExe[100];
};

void getState(data *pro, int processCount){
  for(int i = 0; i <= processCount; i++) {
    if((pro[i].state != "TERMINATED") && (pro[i].timeExe[0] <= currentTime)) {
      if((pro[i].process[pro[i].place] == "CORE") || ((pro[i].process[pro[i+1].place] == ""))) {
        pro[i].state = "RUNNING";
      }else if((pro[i].process[pro[i].place] == "DISPLAY") || (pro[i].process[pro[i].place] == "INPUT")) {
        pro[i].state = "BLOCKED";
      }else{
        pro[i].state = "READY";
      }
    }
  }
}

void coreRequest(data *pro, int &point, int processCount) {
  if(pro[point].state != "TERMINATED") {
    if(ncores > 0) {
      if(pro[point].timeExe[pro[point].place] >= 0) {
        if(ncores1 >= 0 || ncores1 < ncores) {
          ncores1++;
        }
        if(pro[point].timeExe[pro[point].place] <= slice) {
          currentTime += pro[point].timeExe[pro[point].place];
          pro[point].place += 1;
          if(ncores1 > 0) {
            ncores1--;
          }
        }else{
          currentTime += slice;
          pro[point].timeExe[pro[point].place] -= slice;
        }
      }
    }
  }

  if(pro[point].process[pro[point].place] == "DISK") {
    currentTime -= pro[point].timeExe[pro[point].place];
    diskCount += 1;
  }

  getState(pro, processCount);

  for(int i = 0; i <= processCount; i++) {
    if(pro[i+1].process[0] != "") {
      if(pro[i+1].timeExe[0] >= currentTime) {
        readyCount += 1;
      }
    }
  }

  if(point >= processCount) {
    point = 0;
    if(diskCount > 0) {
      diskCount -= 1;
    }
    if(readyCount > 0) {
      readyCount -= 1;
    }
  }else{
    point += 1;
    if(diskCount > 0) {
      diskCount -= 1;
    }
    if(readyCount > 0) {
      readyCount -= 1;
    }
  }
}

void inputCompletion(int doneProcessor, string &status, int processCount, data *pro, int &count) {
  cout << endl;
  count2++;
  count++;
  cout << "Process " << doneProcessor << " terminated at t = " << currentTime << endl;
  if(processCount == count2) {
    ncores1 = 0;
  }
  cout << "Number of busy cores: " << ncores1 << endl;
  cout << "Ready queue contains: ";
  if(readyCount == 0) {
    cout << "--" << endl;
  }else{
    cout << readyCount << endl;
  }
  cout << "Disk queue contains: ";
  if(diskCount == 0) {
    cout << "--" << endl;
  }else{
    cout << diskCount << endl;
  }
  cout << "Process ID\tStart time\tStatus" << endl;
  // loop through every process and prints it
  int number = -1;
  for(int i = 0; i <= processCount; i++) {
    number++;
    if((pro[number].process[i] != "") && (pro[number].state != "TERMINATED")) {
      if(doneProcessor == number) {
        pro[doneProcessor].state = "TERMINATED";
      }
      cout << i << "\t\t" << pro[number].timeExe[0] << "\t\t" << pro[number].state << endl;
    }
  }

  pro[doneProcessor].place += 1;
  if(processCount <= count2) {
    status = "STOP";
  }
  cout << endl;
}

int main() {
  string tempProcess, status = "RUNNING";
  int tempTime, processCount = -1, count = -1, count1 = 0, requestTime = 0;
  int nInputLines = 0, point = 0;
  const int MAXLINES = 2048;
  int tracker[64];

  data processor[64];

  // Give all the process/time each their places
  // in an struct
  while(cin >> tempProcess >> tempTime) {
    if ((nInputLines > MAXLINES) || (processCount > 64)) {
      cout << "Too many input lines for current MAXLINES.\n";
      exit(1);
    }
    nInputLines++;
    if(tempProcess == "NEW") {
      processCount++;
      count1 = 0;
    }
    if(tempProcess == "NCORES") {
      ncores = tempTime;
    }else if(tempProcess == "SLICE") {
      slice = tempTime;
    }else{
      processor[processCount].process[count1] = tempProcess;
      if((processor[processCount].process[count1] == "DISPLAY") || (processor[processCount].process[count1] == "INPUT")) {
        tempTime = 0;
      }
      processor[processCount].timeExe[count1] = tempTime;
      count1++;
    }
  }

  // track each position of the processors
  for(int t = 0; t <= processCount; t++) {
    for(int k = 0; k <= nInputLines; k++) {
      if(processor[t].process[k] == "") {
        break;
      }else{
        tracker[t] += 1;
      }
    }
  }

  // Intialize each processor place to 1 *** ignore NEW and the timeExecution
  for(int z = 0; z <= processCount; z++) {
    processor[z].place = 1;
  }

  while(status == "RUNNING") {
    for(int r = 0; r <= processCount; r++) {
      if(processor[r].place == tracker[r]) {
        inputCompletion(r, status, processCount, processor, count);
      }
    }
    coreRequest(processor, point, processCount);
  }

  return 0;
}
