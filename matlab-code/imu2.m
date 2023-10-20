% comparing gyro, accel and comp filter
clc, clear
A = readmatrix('data7.csv');
B = readmatrix('data9.csv');
C = readmatrix('data10.csv');



t = A(1:end,1) - A(1,1);
dt = .01;
gyro = A(1:end,2);  % raw ax
accel= A(1:end,3); % filted ax
comp = A(1:end,4);

t2 = B(1:end,1) - B(1,1);
encA = B(1:end,2); % encoder 
AccA = B(1:end,3); % accel
FilA = B(1:end,4); % comp filter

t3 = C(1:end,1) - C(1,1);
encAA = -C(1:end,2); % encoder 
a1 = C(1:end,3); % alpha 0.1
a2 = C(1:end,4); % 0.2
a3 = C(1:end,5); % 0.3

color1 = '#332288';
color10 = '#a6cee3';
color11 = '#1f78b4';
color12 = '#b2df8a';
color13 = '#1b9e77';


subplot(1,1,1)
plot(t,accel,'Color',color10,'Linewidth',2), hold on
plot(t,comp,'Color',color13,'Linewidth',2), hold on
plot(t,gyro,'Color',color11,'Linewidth',2.3)
ax = gca;    ax.FontSize = 20;
xlabel('Time (s) ','Linewidth',1.25);
ylabel('Roll (deg)','Linewidth',1.25);
legend('Accelerometer','Complementary','Gyroscope');
box on
xlim([0 10])
ylim([-0.5 0.5])
%}
figure(2);
plot(t2,encA,'Color',color11,'Linewidth',2), hold on
plot(t2,AccA,'Color',color1,'Linewidth',2), hold on
plot(t2,FilA,'Color',color12,'Linewidth',2)
ax = gca;    ax.FontSize = 20;
xlabel('Time (s) ','Linewidth',1.25);
ylabel('Roll (deg)','Linewidth',1.25);
legend('Encoder','Accelerometer','Complementary');
box on
xlim([0 20])
%ylim([-0.5 0.5])

figure(3);
%plot(t3,encAA,'Color',color10,'Linewidth',1.5), hold on
plot(t3,a1,'Color',color11,'Linewidth',2), hold on
plot(t3,a2,'Color',color1,'Linewidth',2), hold on
plot(t3,a3,'Color',color12,'Linewidth',2)
ax = gca;    ax.FontSize = 20;
xlabel('Time (s) ','Linewidth',1.25);
ylabel('Roll (deg)','Linewidth',1.25);
legend('\alpha = 0.05','\alpha = 0.2','\alpha = 0.5');
box on
xlim([0 5])
%ylim([-0.5 0.5])