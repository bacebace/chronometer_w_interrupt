projeto de EA871 (roteiro 8)

nao coloquei os arquivos .h que sao gerados automaticamente pelo CodeWarrior

enunciado: 

Implementar programa cronômetro com mesmas funcionalidades do cronômetro desenvolvido no roteiro 7 (Relógio Tempo Real - sem interrupção), mas agora utilizando o temporizador do Systick, com uso de interrupção, para garantir uma base de tempo confiável. Também diferentemente do roteiro 7, a comunicação serial entre o Terminal e o microcontrolador deve ser realizada a uma taxa de 1200 bps. Note que, dependendo de como você implementar seu programa, ele pode mostrar um valor errado, caso a interrupção ocorra durante o processo de apresentação do valor no LCD e no Terminal.  Por exemplo, caso uma interrupção do Systick (que pode acontecer a qualquer momento) ocorra no meio da apresentação do valor do cronômetro após a apresentação, por exemplo, dos segundos no horário 00:05:99 (05 no caso) e antes da apresentação do valor dos centésimos (99 no caso), pode ser que o programa apresente o valor errado 00:05:00, já a interrupção atualizou o cronômetro para 00:06:00. Como ainda não foi apresentado os centésimos de segundos, ao retornar da rotina de tratamento de interrupção, o processo de apresentação do valor do horário será retomado para a apresentação dos centésimos que agora é 00. Implemente o seu programa para que tal situação não ocorra.

ATENÇÃO: a) a apresentação do valor do cronômetro no LCD e no Terminal não deve ser feito dentro da rotina de tratamento de interrupção já que esta rotina deve ser rápida  para não atrasar o cronômetro; b) para não atrasar o cronômetro, apenas desabilite a interrupção para parar o cronômetro.
