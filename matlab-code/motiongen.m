color1 = '#332288';
color2 = '#88CCEE';
color3 = '#44AA99';
color4 = '#117733';
color5 = '#999933';
color6 = '#DDCC77';
color7 = '#CC6677';
color8 = '#882255';
color9 = '#AA4499';
color10 = '#a6cee3';
color11 = '#1f78b4';
color12 = '#b2df8a';
color13 = '#1b9e77';
color14 = '#d95f02';
color15 = '#e9a3c9';
color16 = '#a1d76a';
color17 = '#d8b365';
color18 = '#5ab4ac';


%% Starting point (need to init the servos so that the A1y is set to this position)
Px = -20;
Py = 0;
A1x = zeros(1,length(Px)); 
A1y = 86.974*ones(1,length(Px));
A2x = zeros(1,length(Px));
A2y = 86.974*ones(1,length(Px));

%% constant values of leg links length. Links are: c1, c2, a1, a2 
c1 = 17.5; % [mm] this is the link connected to servo 1 and c2
c2 = 61.5; % connected to c1 and a2
a1 = 61.5;   % parallelogram, a1 connected to servo 2 and a2
a2 = 79;   % link that touches the ground

%% solve for b : the resultant vector from the origin to the end effector
d = A1y - Py;
e = Px;
b = sqrt(d.^2 + e.^2);

%% get the 2 angles that need to be sent to the servos (psi and eta)
S = acos(d./b); % angle between b and the vertical d "y component of b"
if e < 0
    S = -S; % if e is negative make the angle S negative
end
alpha = acos((b.^2 + a1.^2 - (a2-c1).^2)/(2.*b.*a1));
g = (pi./2) - S; % angle from right horizontal to b
psi = pi - alpha - g; % 180 deg minus the two angles
beta = pi - 2.*alpha; % isoceles triangle: so beta the angle from a2 and a1 is easy to get from alpha
phi = beta - psi; %angle between left horizontal to c1
eta = (pi./2) - phi;
%% convert (psi and eta into degrees)
psi_deg = (180./pi).*psi
eta_deg = (180./pi).*eta

%% Animation
fig = figure;

%% Define rest of joint positions
one = ones(1,length(psi_deg));
for i = 1:length(psi_deg)
    
    Bx(i) = A1x(i) - c1*sind(eta_deg(i));
    By(i) = A1y(i) + c1*cosd(eta_deg(i));
    Cx(i) = Bx(i) - c2*cosd(psi_deg(i));
    Cy(i) = By(i) - c2*sind(psi_deg(i));
    Dx(i) = A2x(i) - a1*cosd(psi_deg(i));% + a1*sind(eta_deg(i));
    Dy(i) = A2y(i) - a1*sind(psi_deg(i));% - a1*cosd(eta_deg(i));
    
    DDx(i) = Dx(i) + a1*sind(eta_deg(i));
    DDy(i)  = Dy(i) - a1*cosd(eta_deg(i));
    cc1x = [A1x(i) Bx(i)];
    cc1y = [A1y(i) By(i)];
    aa1x = [A1x(i) Dx(i)];
    aa1y = [A1y(i) Dy(i)];
    plot( cc1x, cc1y, [Bx(i) Cx(i)], [By(i) Cy(i)],[Cx(i) Dx(i)], [Cy(i) Dy(i)], 'LineWidth', 5)
    hold on
    plot( [A1x(i) Bx(i)], [A1y(i) By(i)], 'LineWidth', 5)
    hold on
    %plot( [A1x(i) Dx(i)], [A1y(i) Dy(i)], 'LineWidth', 2)
    plot( [A1x(i) Dx(i)], [A1y(i) Dy(i)], [Dx(i) DDx(i)], [Dy(i) DDy(i)], 'LineWidth', 5) 
    xlim([-80 40])
    ylim([-15 115])
    xlabel('x (mm)','Linewidth',24);
    ylabel('y (mm)','Linewidth',24);
    grid("on")
    ax = gca;    ax.FontSize = 24;
    %flipBook(i) = getframe(fig); % force graphics to render also returns as a image
    drawnow
    hold off
end

sx = Bx - Cx
sy = By - Cy
magcheck = sqrt((sx).*(sx) + (sy).*(sy))

