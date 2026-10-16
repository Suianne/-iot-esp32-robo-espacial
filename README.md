# Robô Explorador Espacial — Controle Remoto Simulado

## 1. Nome do projeto

**Controle Remoto de Robô Explorador Espacial com ESP32-S3**

## 2. Objetivo da etapa

Desenvolver e simular, no Wokwi, um circuito de controle remoto utilizando o ESP32-S3, um joystick analógico, dois LEDs e um botão.

O objetivo é testar a leitura do joystick para identificar quatro direções de movimentação, simular o envio de comandos, verificar o funcionamento do botão de desligamento e utilizar os LEDs para indicar o estado do controle.

Os comandos previstos são:

- **Frente:** comando de movimentação para frente.
- **Trás:** comando de movimentação para trás.
- **Esquerda:** comando de movimentação para a esquerda.
- **Direita:** comando de movimentação para a direita.
- **Desligar:** comando `DESLIGAR`.

As mensagens correspondentes são exibidas no Monitor Serial durante a simulação.

## 3. Lista de componentes do circuito

| Componente | Quantidade | Função |
|---|---:|---|
| ESP32-S3 | 1 | Executar o programa do controle remoto |
| Joystick analógico | 1 | Identificar as direções selecionadas |
| LED verde | 1 | Indicar o estado normal do controle |
| LED vermelho | 1 | Indicar o estado de desligamento ou indisponibilidade |
| Resistor de 220 Ω | 2 | Limitar a corrente dos LEDs |
| Botão de pressão | 1 | Simular a solicitação de desligamento |
| Jumpers virtuais | Conforme necessário | Realizar as conexões |
| Protoboard virtual | 1 | Organizar os componentes do circuito |

## 4. Como rodar no Wokwi

### 4.1. Criar o circuito

1. Acesse [https://wokwi.com](https://wokwi.com).
2. Crie um novo projeto com uma placa ESP32-S3 compatível.
3. Adicione o joystick analógico, os LEDs, os resistores e o botão de pressão.
4. Conecte os componentes aos pinos definidos no código do controle remoto.

### 4.2. Configurar o código

1. Abra o arquivo `sketch.ino`.
2. Insira o código desenvolvido no Arduino IDE.
3. Confira se os pinos configurados no código correspondem às ligações do circuito.
4. Salve o projeto.

### 4.3. Executar a simulação

1. Clique em **Start Simulation**.
2. Abra o Monitor Serial e configure a velocidade para 115200 baud.
3. Movimente o joystick nas quatro direções e observe os comandos exibidos.
4. Pressione o botão e verifique a mensagem referente ao comando `DESLIGAR`.
5. Observe o comportamento dos LEDs durante os testes.

## 5. Resultado esperado

Ao final da etapa, espera-se que o circuito simulado permita testar a leitura do joystick, a identificação das quatro direções, o acionamento do botão de desligamento e a sinalização por LEDs, com as mensagens correspondentes apresentadas no Monitor Serial.

## 6. Link para a simulação
Link: https://velxio.dev/angelo-oliveira/joystick/ 

