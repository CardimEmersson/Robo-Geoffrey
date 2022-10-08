#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <Robojax_L298N_DC_motor.h>

// configurações do sensor
#define VELOCIDADE_DO_SOM 0.034
#define TRIGGER_PIN 23
#define ECHO_PIN 22

// configurações do motor 1
#define CHA 0
#define ENA 19
#define IN1 18
#define IN2 5

// configurações do motor 2
#define IN3 17
#define IN4 16
#define ENB 4
#define CHB 1

#define MOTOR_1 1
#define MOTOR_2 2

const int MOTOR_PARA_FRENTE = 2;
const int MOTOR_PARA_TRAS = 1;

const int VELOCIDADE_PADRAO = 100;
const int VELOCIDADE_ARRANCADA = 70;
const int DISTANCIA_LIMITE = 40;

// setup inicial
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA, IN3, IN4, ENB, CHB);

void setup()
{

  Serial.begin(115200);
  Dabble.begin("Robo Jenkins");
  robot.begin();

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop()
{
  Dabble.processInput();

  controleDirecionalDigital();
  controleDirecionalAnalogico();
  controleAcoes();
}

int calculaPWM(int velocidadePWM)
{

  if (velocidadePWM >= 30 && velocidadePWM <= 70)
  {
    return VELOCIDADE_PADRAO;
  }

  if (velocidadePWM > 70 && velocidadePWM <= 145)
  {
    return VELOCIDADE_PADRAO + 20;
  }

  if (velocidadePWM > 145)
  {
    return VELOCIDADE_PADRAO + 30;
  }

  return 0;
}

void moveParaFrente(int velocidade)
{
  robot.rotate(MOTOR_1, velocidade, MOTOR_PARA_FRENTE);
  robot.rotate(MOTOR_2, velocidade, MOTOR_PARA_FRENTE);
}

void moveParaTras(int velocidade)
{
  robot.rotate(MOTOR_1, controle, MOTOR_PARA_TRAS);
  robot.rotate(MOTOR_2, controle, MOTOR_PARA_TRAS);
}

void moveParaDireita(int velocidade)
{
  robot.rotate(MOTOR_1, calculaPWM(velocidade), MOTOR_PARA_FRENTE);
  robot.rotate(MOTOR_2, calculaPWM(velocidade), MOTOR_PARA_TRAS);
}

void moveParaEsquerda(int velocidade)
{
  robot.rotate(MOTOR_1, calculaPWM(velocidade), MOTOR_PARA_TRAS);
  robot.rotate(MOTOR_2, calculaPWM(velocidade), MOTOR_PARA_FRENTE);
}

void pararMotor()
{
  robot.brake(MOTOR_1);
  robot.brake(MOTOR_2);
}

void controleDirecionalDigital()
{

  bool colisaoFrontal = verificaColisao(TRIGGER_PIN, ECHO_PIN, DISTANCIA_LIMITE);

  if (colisaoFrontal)
  {
    pararMotor();
    return;
  }

  if (GamePad.isUpPressed())
  {
    for (int controleVelocidade = VELOCIDADE_ARRANCADA; VELOCIDADE_PADRAO <= controleVelocidade; controleVelocidade += 10)
    {
      moveParaFrente(controleVelocidade);
    }
    return;
  }

  if (GamePad.isDownPressed())
  {
    for (int controleVelocidade = VELOCIDADE_ARRANCADA; VELOCIDADE_PADRAO <= controleVelocidade; controleVelocidade += 10)
    {
      moveParaTras(controleVelocidade);
    }
    return;
  }

  if (GamePad.isLeftPressed())
  {
    for (int controleVelocidade = VELOCIDADE_ARRANCADA; VELOCIDADE_PADRAO <= controleVelocidade; controleVelocidade += 10)
    {
      moveParaEsquerda(controleVelocidade);
    }
    return;
  }

  if (GamePad.isRightPressed())
  {
    for (int controleVelocidade = VELOCIDADE_ARRANCADA; VELOCIDADE_PADRAO <= controleVelocidade; controleVelocidade += 10)
    {
      moveParaDireita(controleVelocidade);
    }
    return;
  }

  pararMotor();
}

int velocidade_PWM_y = 0;
int velocidade_PWM_x = 0;

void controleDirecionalAnalogico()
{
  bool colisaoFrontal = verificaColisao(TRIGGER_PIN, ECHO_PIN, DISTANCIA_LIMITE);

  if (colisaoFrontal)
  {
    pararMotor();
    return;
  }

  int angulo = GamePad.getAngle();
  int raio = GamePad.getRadius();
  float eixoX = GamePad.getXaxisData();
  float eixoY = GamePad.getYaxisData();

  velocidade_PWM_y = map(eixoY, -7, 7, -255, 255);
  velocidade_PWM_x = map(eixoX, -7, 7, -255, 255);

  if (velocidade_PWM_y > 0 && velocidade_PWM_x == 0)
  {
    moveParaFrente(calculaPWM(velocidade_PWM_y));
    return;
  }

  if (velocidade_PWM_y < 0 && velocidade_PWM_x == 0)
  {
    moveParaTras(calculaPWM(velocidade_PWM_y * -1));
    return;
  }

  if (velocidade_PWM_y >= 0 && velocidade_PWM_x < 0)
  {
    moveParaDireita(calculaPWM(VELOCIDADE_PADRAO));
    return;
  }

  if (velocidade_PWM_y >= 0 && velocidade_PWM_x > 0)
  {
    moveParaEsquerda(calculaPWM(VELOCIDADE_PADRAO));
    return;
  }

  if (velocidade_PWM_y < 0 && velocidade_PWM_x < 0)
  {
    moveParaDireita(calculaPWM(VELOCIDADE_PADRAO));
    return;
  }

  if (velocidade_PWM_y < 0 && velocidade_PWM_x > 0)
  {
    moveParaEsquerda(calculaPWM(VELOCIDADE_PADRAO));
    return;
  }

  if (velocidade_PWM_y == 0 && velocidade_PWM_x == 0)
  {
    pararMotor();
    return;
  }
}

void controleAcoes()
{
  if (GamePad.isSquarePressed())
  {
    Serial.print("Square");
  }

  if (GamePad.isCirclePressed())
  {
    Serial.print("Circle");
  }

  if (GamePad.isCrossPressed())
  {
    Serial.print("Cross");
  }

  if (GamePad.isTrianglePressed())
  {
    Serial.print("Triangle");
  }

  if (GamePad.isStartPressed())
  {
    pararMotor();
  }

  if (GamePad.isSelectPressed())
  {
    Serial.println("Select");
  }
}

bool verificaColisao(int trigger, int echo, int distanciaLimite)
{
  float distanciaCalculada = calculaDistancia(TRIGGER_PIN, ECHO_PIN);

  return distanciaLimite >= distanciaCalculada;
}

float calculaDistancia(int trigger, int echo)
{
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);

  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigger, LOW);

  long duracao = pulseIn(echo, HIGH);

  float distanciaCm = duracao * VELOCIDADE_DO_SOM / 2;

  return distanciaCm;
}