#define INPUTS 6
#define HIDDEN 20
#define OUTPUTS 4
#define STEPS 40

#define TDS_MIN 297.2
#define TDS_MAX 2731.4

#define TURB_MIN 1196.6
#define TURB_MAX 3143.8

#define FLOW_MIN 0.0
#define FLOW_MAX 4.27

float beta = 0.9;
float threshold = 1.0;

#include <WiFi.h>

const char* ssid = "ESP_WIFI";
const char* password = "xyz12345";

WiFiClient client;



/* ---------------- MODEL WEIGHTS ---------------- */

const float W1[INPUTS][HIDDEN] = {-0.18438801169395447,0.18697167932987213,-0.14409001171588898,-0.5371074676513672,0.31345704197883606,-0.4543129801750183,-0.5144999623298645,-0.5304962992668152,-0.7560247182846069,-0.2569825053215027,-0.13561737537384033,-0.3413980305194855,0.4366835355758667,-1.2079559564590454,0.21843321621418,0.10411785542964935,0.13949351012706757,0.7857503294944763,-0.5180104374885559,-0.33043187856674194,0.07693228870630264,-0.5292026400566101,-0.791577935218811,-0.46436917781829834,-0.704567015171051,-0.5111594796180725,0.3632313907146454,-0.20412039756774902,-0.11011061817407608,-0.1396598070859909,-0.180378720164299,-0.2599542438983917,-2.348544120788574,-0.15308606624603271,0.3041630685329437,-2.1508750915527344,-0.2005748152732849,0.26166099309921265,-0.5382052659988403,0.06144079938530922,-0.12475105375051498,-0.9340478181838989,-1.5740736722946167,0.2802339494228363,0.9000523686408997,0.18220655620098114,-0.8443607091903687,0.6526517868041992,0.02850482426583767,0.28511735796928406,-1.290753960609436,-0.2553408741950989,-0.7949424982070923,-1.4555584192276,0.21596239507198334,1.1330777406692505,0.1789674609899521,-0.027689136564731598,-0.7317272424697876,-0.3301440179347992,-0.11576677858829498,-0.5827550888061523,-0.6346395015716553,0.07539134472608566,0.07792546600103378,-0.40737003087997437,0.26382747292518616,-0.43674594163894653,-0.4068116843700409,-0.3870775103569031,-0.01291818730533123,0.15824735164642334,-0.9475314021110535,-0.5247138142585754,0.09483575820922852,0.27782344818115234,-0.06210727617144585,0.306299090385437,-0.2282627522945404,0.2225804328918457,-0.1022813692688942,0.10564623773097992,-0.2732861042022705,-0.15444524586200714,0.12557412683963776,0.41468384861946106,-0.011396683752536774,-0.24199435114860535,-0.12703460454940796,-0.03591971471905708,0.3375818133354187,-0.25438588857650757,0.13803458213806152,-0.6456694602966309,0.13279293477535248,-0.31732475757598877,0.6272000670433044,-0.07657191902399063,-0.13711023330688477,0.04440087452530861,-0.2823602557182312,0.4152148962020874,-0.06555277854204178,0.4630783498287201,-0.33465391397476196,0.1399155557155609,-0.5360259413719177,-0.3021714687347412,0.5179823637008667,-0.03392579033970833,0.14050425589084625,0.12263178825378418,0.2850928008556366,-0.004549980629235506,-1.7605557441711426,0.7975583672523499,0.36117085814476013,0.1988621950149536,-0.10924780368804932,0.2108376920223236};

float BIAS1[HIDDEN] = {-0.3003571927547455,-0.3432300090789795,0.2707284390926361,0.11274721473455429,-0.4312915802001953,0.29143333435058594,0.5384700298309326,-0.3894503712654114,0.4171178638935089,-0.32216495275497437,0.3639242351055145,-0.5675051808357239,0.4504263997077942,-0.13757076859474182,-0.05930270627140999,-0.3277238607406616,-0.13832610845565796,0.026116786524653435,-0.12218300253152847,-0.5559678673744202};

const float W2[HIDDEN][OUTPUTS] = {-0.49155935645103455,-0.29625841975212097,-0.07933462411165237,0.12795062363147736,0.12477909028530121,-0.6615552306175232,-0.24103547632694244,-0.23443062603473663,0.3654307425022125,-0.04859429970383644,-0.2980719208717346,-0.16590166091918945,0.6968160271644592,0.14466668665409088,0.19723133742809296,0.1000058725476265,0.16310164332389832,0.1883024275302887,0.08176770061254501,-0.6675673723220825,-0.28842294216156006,-0.049597758799791336,0.4815467894077301,0.03962866961956024,-0.052987951785326004,-0.23344752192497253,0.21483078598976135,-0.7257276773452759,0.24866913259029388,0.6606889367103577,-0.18662329018115997,-0.16608741879463196,0.4343077838420868,0.032187312841415405,0.28778791427612305,-0.08574990928173065,0.25351905822753906,0.3027770519256592,0.14415660500526428,0.4993606209754944,0.003962357062846422,-0.230159193277359,-0.9539206624031067,0.01958669163286686,-0.2926071584224701,-0.1290120929479599,0.4322366714477539,-0.7680647373199463,0.564764678478241,-0.10240399837493896,0.08648336678743362,0.14184239506721497,-0.05878283083438873,-0.02814594656229019,0.16856837272644043,-0.021837370470166206,-0.014008298516273499,0.17053799331188202,0.08155644685029984,-0.03150977939367294,0.041277822107076645,0.6152365207672119,-0.32289475202560425,-0.04547295346856117,-0.5810962319374084,0.22081144154071808,0.42664578557014465,-0.5837282538414001,0.08791846036911011,0.024446014314889908,0.2560786306858063,-0.17199911177158356,-0.08803378790616989,-0.06522461026906967,0.13004960119724274,-0.015886634588241577,-0.2336762696504593,0.20245347917079926,-0.1667880266904831,-0.04457993805408478};

float BIAS2[OUTPUTS] = {0.2454550862312317,-0.17847861349582672,0.236428901553154,0.27601149678230286};

/* ---------------- STATES ---------------- */

float mem1[HIDDEN];
float mem2[OUTPUTS];
int spike_count[OUTPUTS];


/* ---------------- SENSOR FUNCTIONS ---------------- */

float readTDS() {
  return analogRead(35);
}

float readTurbidity() {
  return analogRead(34);
}

/* ---------------- FLOW SENSOR ---------------- */

volatile int flowPulse = 0;
unsigned long lastFlowTime = 0;

void IRAM_ATTR flowInterrupt() {
  flowPulse++;
}

float readFlow() {

  unsigned long currentTime = millis();
  float timeInterval = (currentTime - lastFlowTime) / 1000.0;   // seconds

  noInterrupts();
  int pulses = flowPulse;
  flowPulse = 0;
  interrupts();

  lastFlowTime = currentTime;

  if(timeInterval <= 0) return 0;

  // YF-S201 sensor formula
  float flowRate = (pulses / 7.5) / timeInterval;   // L/min

  return flowRate;
}



/* ---------------- SMOOTHING ---------------- */

float smoothTDS(float val){
  static float buf[5]; static int idx=0;
  buf[idx]=val; idx=(idx+1)%5;
  float sum=0;
  for(int i=0;i<5;i++) sum+=buf[i];
  return sum/5;
}

float smoothTurbidity(float val){
  static float buf[5]; static int idx=0;
  buf[idx]=val; idx=(idx+1)%5;
  float sum=0;
  for(int i=0;i<5;i++) sum+=buf[i];
  return sum/5;
}

/* ---------------- NORMALIZATION ---------------- */

float normalize(float value,float min_v,float max_v){
  float n=(value-min_v)/(max_v-min_v);
  if(n<0) n=0;
  if(n>1) n=1;
  return n;
}

/* ---------------- LIF NEURON ---------------- */

int lif(float input,float *mem){
  *mem=beta*(*mem)+input;
  if(*mem>threshold){
    *mem=0;
    return 1;
  }
  return 0;
}

/* ---------------- SNN ---------------- */

void run_snn(float x[INPUTS]){

  for(int i=0;i<HIDDEN;i++) mem1[i]=0;
  for(int i=0;i<OUTPUTS;i++){ mem2[i]=0; spike_count[i]=0; }

  for(int t=0;t<STEPS;t++){

    int hidden_spk[HIDDEN];

    for(int j=0;j<HIDDEN;j++){
      float sum = BIAS1[j];
      for(int i=0;i<INPUTS;i++)
        sum+=x[i]*W1[i][j];
      hidden_spk[j]=lif(sum,&mem1[j]);
    }

    for(int k=0;k<OUTPUTS;k++){
      float sum = BIAS2[k];
      for(int j=0;j<HIDDEN;j++)
        sum+=hidden_spk[j]*W2[j][k];

      int spk=lif(sum,&mem2[k]);
      spike_count[k]+=spk;
    }
  }
}

int predict(){
  int best=0;
  for(int i=1;i<OUTPUTS;i++)
    if(spike_count[i]>spike_count[best])
      best=i;
  return best;
}

/* ---------------- LOW POWER EVENT TRIGGER ---------------- */



void connectWiFi(){

  if(WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");

  int timeout = 0;

  while(WiFi.status() != WL_CONNECTED && timeout < 20){
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("\nWiFi connected");
  }
  else{
    Serial.println("\nWiFi failed");
  }

}


void sendAlert(){

  if(WiFi.status() == WL_CONNECTED){

    Serial.println("Sending alert to server...");

    if(client.connect("10.135.225.66", 5000)){
      client.println("BLOCKAGE ALERT");
      client.flush();        // ensure data sent
      delay(200);            // allow transmission
      client.stop();
      Serial.println("Alert sent");
    }
    else{
      Serial.println("Server connection failed");
    }

  }

}




float prev_flow=0;
float last_tds=0;
float last_turb=0;


void setup(){
  Serial.begin(115200);

  pinMode(27, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(27), flowInterrupt, RISING);

  lastFlowTime = millis();
}


void loop(){

  Serial.println("Loop running...");
  float TDS=readTDS();
  float turb=readTurbidity();
  float flow=readFlow();

  /* Event trigger: run only when sensor changes */

  if(abs(TDS-last_tds)>20 || abs(turb-last_turb)>20 || abs(flow-prev_flow)>0.1){

    last_tds=TDS;
    last_turb=turb;

    /* smoothing */
    TDS=smoothTDS(TDS);
    turb=smoothTurbidity(turb);

    /* normalization */
    TDS=normalize(TDS,TDS_MIN,TDS_MAX);
    turb=normalize(turb,TURB_MIN,TURB_MAX);
    flow=normalize(flow,FLOW_MIN,FLOW_MAX);

    /* features */
    float flow_delta=flow-prev_flow;
    float flow_low=(flow<0.2)?1:0;
    float flow_ratio = flow/(turb + 0.01);

    prev_flow=flow;

    float input[6]={TDS,turb,flow,flow_delta,flow_low,flow_ratio};

    run_snn(input);

    int label=predict();  
    /* DEBUG PRINTS */

    Serial.print("TDS: ");
    Serial.print(TDS);

    Serial.print("  Turb: ");
    Serial.print(turb);

    Serial.print("  Flow: ");
    Serial.print(flow);

    Serial.print("  Label: ");
    Serial.println(label);

    if(label==3){

    Serial.println("BLOCKAGE ALERT!");

    connectWiFi();
    sendAlert();

    }

  }

  esp_sleep_enable_timer_wakeup(5 * 1000000);
  esp_light_sleep_start();
}