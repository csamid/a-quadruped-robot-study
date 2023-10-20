% IMU data, comparing comp filter to accel and gyro
clc, clear
A = readmatrix('3_12_2022_h_o.txt');
B = readmatrix('3_12_2022_h2_o.txt');


f = figure(1)
phi_hat_comp = A(3:end,1);
phi_hat_accel = A(3:end,3);
phi_hat_gyro = A(3:end,5);
t = A(3:end,7);

color2 = '#88CCEE';
color4 = '#117733';
color8 = '#882255';

plot(t,phi_hat_accel,'Color',color2,'Linewidth',1.50)
box on
hold on
%%plot(t,phi_hat_accel,'Color',color2,'Linewidth',1.50)
plot(t,phi_hat_gyro,'Color',color4,'Linewidth',1.50)
plot(t,phi_hat_comp,'Color',color8,'Linewidth',1.50)
ax = gca;    ax.FontSize = 18;
ylim([-2 2])
xlim([0 30])
xlabel('Time (sec) ','FontSize',22,'Linewidth',1.25);
ylabel('Pitch (deg)','FontSize',22,'Linewidth',1.25);
legend('Accelerometer','Gyroscope','Complementary','FontSize',22);
f = sum(255 - f, 3);
figure(2)
phi_hat_comp2 = B(3:end,2);
phi_hat_accel2 = B(3:end,4);
phi_hat_gyro2 = B(3:end,6);
t2 = B(3:end,7);
plot(t2,phi_hat_accel2,'Color',color2,'Linewidth',1.50)
box on
hold on
plot(t2,phi_hat_gyro2,'Color',color4,'Linewidth',1.50)
plot(t2,phi_hat_comp2,'Color',color8,'Linewidth',1.50)
ax = gca;    ax.FontSize = 18;
%ylim([-2 2])
%xlim([0 30])
xlabel('Time (sec) ','FontSize',22,'Linewidth',1.25);
ylabel('Pitch (deg)','FontSize',22,'Linewidth',1.25);
legend('Accelerometer','Gyroscope','Complementary','FontSize',22);
