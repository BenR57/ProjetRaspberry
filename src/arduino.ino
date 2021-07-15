// Le temps est compté en millisecondes

// Intialisation des variables globales
const short int Lmin = 7;   // Pin de la LED verte le plus à droite
const short int Lmax = 12;  // Pin de la LED rouge le plus à gauche
const short int pinSon = 5; // Pin du capteur de son
const unsigned long routineTimeSound = 1500;
const unsigned long routineTimePresence = 1500;
short int testMode = 0;
String msg = "";

void DisplayLights(char , int); // Fonction permettant d'allumer ou d'eteindre les LED 2 par 2

//Déclaration Classes 


//Classe de base des routines d'affichage lumineux à l'aide des LEDs

class Routine{
  
  protected:
    unsigned long startingTime, endingTime; // Debut et fin de l'intervale de detection
    unsigned long totalRoutineTime;         // Temps total d'affichage de la routine
  
  public:
    // Constructeur
    Routine(){
      startingTime = millis();
      endingTime = millis();
      };
    virtual ~Routine()=default;
    unsigned long GetTotalRoutineTime(){return totalRoutineTime;};
    unsigned long GetStartingTime(){return startingTime;};
    void AddRoutineTime(){
      unsigned long tmp = millis();
      totalRoutineTime += tmp-endingTime;     // Ajoute la durée additionalTime à durée d'existence de l'objet
      endingTime = tmp;                       // Update la durée de fin du signal pour les logs
    };
};



class RoutineAffichage : public Routine{
  
  protected:
    short int routineType;                  // Type de la routine {0:IDLE, 1:Affichage son, 2:Affichage présence}

  public:
    // Constructeur
    RoutineAffichage(){
      DisplayLights(' ', 0);
      };
    short int GetRoutineType(){return routineType;};
    virtual void DisplaySequence() = 0;  // Fonction d'affichage lumineux
};


// ===== IDLE ====== 

//Routine du chenillard (attente réception signal)
class Chenillard : public RoutineAffichage{
  
  private:
    unsigned long currentTime, previousTime;
    short int activeLED=Lmin;                 // Pin de la LED active lors de l'affichage chenillard
    bool switchLight=0;                       // Boooleen donnant l'état on/off
    const long intervalTime = 200;            // Durée d'un intervale de clignottement
    
  public:
    // Constructeur
    Chenillard(){ 
      totalRoutineTime=0;
      routineType = 0;
      previousTime=millis(); 
    };
    void DisplaySequence() override;
};
void Chenillard::DisplaySequence(){

  digitalWrite(activeLED, switchLight);
  currentTime = millis();
  if( (currentTime-previousTime) > intervalTime) {
    switchLight = !switchLight;
    previousTime = currentTime;
    if (switchLight)
      activeLED ++;
    if (activeLED > Lmax)
      activeLED=Lmin;
  };
}


// ===== Capteur Presence ====== 

class RoutinePresence : public RoutineAffichage{
  
  private:
    unsigned long referenceTime, previousTime;
    bool switchLight = 1;
    bool subRoutineActive = 1;
    const unsigned long subRoutineTime = 5000;
    const unsigned long shortBlinkingTime = 200;
    const unsigned long longBlinkingTime = 2000;
  
  public:
    RoutinePresence(){
      totalRoutineTime = routineTimePresence;
      routineType = 2;
      referenceTime = millis(); 
      DisplayLights('j', switchLight);
    };
    void DisplaySequence();
};
void RoutinePresence::DisplaySequence(){

  if(millis() - referenceTime > subRoutineTime){
    // Si on est dans la 1ere subroutine mais qu'on en a depassé la durée
    // On change certaines parametres afin de rentrer dans la 2nd subroutine
    DisplayLights(' ', 0);
    referenceTime = millis();
    previousTime = millis();
    switchLight = 1; 
    if(subRoutineActive){
      subRoutineActive = 0;
      DisplayLights('v', 1);
    }
    else{
      subRoutineActive = 1;
      DisplayLights('j', 1);
    }
     
  }
  
  if(subRoutineActive){
    if(millis()-previousTime > shortBlinkingTime){
      switchLight = !switchLight;
      previousTime = millis();
      DisplayLights('j', switchLight);
    }
  }
  else if(millis()-previousTime > longBlinkingTime){
      switchLight = !switchLight;
      previousTime = millis();
      DisplayLights('r', !switchLight);
      DisplayLights('v', switchLight);
  }
};


// ===== Capteur Son ====== 

class RoutineSon : public RoutineAffichage{ 
  private: 
    bool switchLight = 0;           // Boooleen donnant l'état on/off
    unsigned long previousTime;
    const long intervalTime = 200;  // Durée d'un intervale de clignottement
    
  public:
    RoutineSon(){ 
      totalRoutineTime = routineTimeSound;          //Durée total de la routine RoutineSon
      routineType = 1;
      previousTime=millis();
    };
  void DisplaySequence() override;
};

void RoutineSon::DisplaySequence() { 
  unsigned long currentTime = millis();  
  if((currentTime-previousTime)>intervalTime) {     
    switchLight = !switchLight;                   // Inverse l'éclairage des LEDs
    previousTime = currentTime;
    DisplayLights('r', !switchLight);             
    DisplayLights('v', switchLight);              
  };
};

// Classes permettant de logger les events son et presence

class LogSon : public Routine{ 
    
  public:
    LogSon(){totalRoutineTime = routineTimeSound;};
    ~LogSon() {
      Serial.print("Son:");
      Serial.print(GetTotalRoutineTime());
      Serial.print(":");
      Serial.print(routineTimeSound);
    };
};

class LogPresence : public Routine{ 
    
  public:
    LogPresence(){totalRoutineTime = routineTimePresence;};
    ~LogPresence() {
      Serial.print("Presence:");
      Serial.print(GetTotalRoutineTime());
      Serial.print(":");
      Serial.print(routineTimePresence);
    };
};



// ==============================================================================================
// ==============================================================================================
// ==============================================================================================
// ==============================================================================================

RoutineAffichage *objRoutineAffichage = new Chenillard();     // On fait pointer notre objet routine sur une routine chenillard
Routine *objLogSon = nullptr;
Routine *objLogPresence = nullptr;



//Initialisation du port serial des pin LEDs et du pin RoutineSon
void setup()
{
  Serial.begin(9600); // Setup du baudrate pour la connexion en RXTX
  for(int i = Lmin; i <= Lmax; i++){pinMode(i, OUTPUT);}  // Setup des LEDs
  pinMode(pinSon, INPUT);                                 // Setup du capteur de son 

}



//Boucle d'action
void loop()
{ 
    if(objRoutineAffichage->GetRoutineType() > 0 && millis() - objRoutineAffichage->GetStartingTime() > objRoutineAffichage->GetTotalRoutineTime()){
      delete objRoutineAffichage;
      objRoutineAffichage = nullptr;
      objRoutineAffichage = new Chenillard();
    }
    if(objLogSon){
      if(millis() - objLogSon->GetStartingTime() > objLogSon->GetTotalRoutineTime()){
        delete objLogSon;
        objLogSon = nullptr;
      }
    }
    if(objLogPresence){
      if(millis() - objLogPresence->GetStartingTime() > objLogPresence->GetTotalRoutineTime()){
        delete objLogPresence;
        objLogPresence = nullptr;
      }
    }
 

    // digitalRead(pinSon) = 1 si on detecte rien et 0 si on detecte du son
    if(!digitalRead(pinSon)){
      // Gestion des logs
      if(objLogSon != nullptr){
        objLogSon->AddRoutineTime();
      }
      else{
        objLogSon = new LogSon();
      }
      // Gestion de l'affichage
      if(objRoutineAffichage->GetRoutineType()==1){
        objRoutineAffichage->AddRoutineTime();
      } 
      if(objRoutineAffichage->GetRoutineType()==0){
        delete objRoutineAffichage;
        objRoutineAffichage = nullptr;
        objRoutineAffichage = new RoutineSon();
      } 
    }

    if(PresenceDetected()){
      // Gestion des logs
      if(objLogPresence != nullptr){
        objLogPresence->AddRoutineTime();
      }
      else{
        objLogPresence = new LogPresence();
      }
      // Gestion de l'affichage
      if(objRoutineAffichage->GetRoutineType()!=2){
          delete objRoutineAffichage;
          objRoutineAffichage = nullptr;
          objRoutineAffichage = new RoutinePresence();
      } 
      else
        objRoutineAffichage->AddRoutineTime();
    }
     
    objRoutineAffichage->DisplaySequence(); 

}





void DisplayLights(char color, int onOff){
  // Fonction permettant d'allumer ou d'eteindre les deux lumières d'une même couleur à la fois
  // La couleur de la lumière est controlée par la variable char color et l'état par la variable int onOff
  // char color = {'r':rouge, 'j':jaune, 'v':vert, default: agit sur tous les lumières}
  // int onOff = {1:allumées, 0:eteintes}
  // Lumières rouges : pin D12 et D11
  // Lumières jaunes : pin D10 et D9
  // Lumières vertes : pin D8 et D7
  
  switch (color) {
    case 'r':
      digitalWrite(12, onOff);
      digitalWrite(11, onOff);
      break;
    case 'j':
      digitalWrite(10, onOff);
      digitalWrite(9, onOff);
      break;
    case 'v':
      digitalWrite(8, onOff);
      digitalWrite(7, onOff);
      break;
    default:
      // On allume/eteint tutes les lumières en même temps
      DisplayLights('r', onOff);
      DisplayLights('j', onOff);
      DisplayLights('v', onOff);
  }
}

bool PresenceDetected() {
  short int msg = 0;
  // On n'attend qu'un seul character depuis le raspberry
  // Ca doit etre 0 ou 1
  if (Serial.available()) {
    msg = Serial.read();
    Serial.flush();
  }
  return msg;
}
