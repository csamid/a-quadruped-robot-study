% leg inverse kinematics
function [eta_deg, psi_deg, S] = inverse_kinematics()
%% Starting point (need to init the servos so that the A1y is set to this position)
Px = -20;
Py = 0;
A1x = 0; 
A1y = 91.92;
A2x = 0;
A2y = 91.92;

%% constant values of leg links length. Links are: c1, c2, a1, a2 
c1 = 20.5; % [mm] this is the link connected to servo 1 and c2
c2 = 65; % connected to c1 and a2
a1 = 65;   % parallelogram, a1 connected to servo 2 and a2
a2 = 85.5;   % link that touches the ground

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
psi_deg = (180./pi).*psi;
eta_deg = (180./pi).*eta;

    
end



    
    





