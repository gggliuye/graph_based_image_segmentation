% tv_img_interp.m
% Total variation image interpolation.
% EE364a
% Defines m, n, Uorig, Known.

% Load original image.
% Uorig = double(imread('tv_img_interp.png'));
clc; clear;

load('depth_raw.mat');
Uorig = double(depth_raw);

gamma = 2;
[m, n] = size(Uorig);

% Create mask of known pixels.
rand('state', 1029);
Known_1 = rand(m,n) > 0.95;
Known_2 = Uorig>100;
Known = Known_1 | Known_2;

%%%%% Put your solution code here

% Calculate and define Ul2 and Utv.

% Placeholder:
Ul2 = ones(m, n);
Utv = ones(m, n);

cvx_begin
    variable Ul2(m, n);
    Ul2(Known) == Uorig(Known); % Fix known pixel values.
    Ux = Ul2(2:end,2:end) - Ul2(2:end,1:end-1); % x (horiz) differences
    Uy = Ul2(2:end,2:end) - Ul2(1:end-1,2:end); % y (vert) differences
    minimize(norm([Ux(:); Uy(:)], 2));% + gamma*norm(Ul2(Known)-Uorig(Known),2)); % l2 roughness measure
cvx_end

cvx_begin
    variable Utv(m, n);
    Utv(Known) == Uorig(Known); % Fix known pixel values.
    Ux = Utv(2:end,2:end) - Utv(2:end,1:end-1); % x (horiz) differences
    Uy = Utv(2:end,2:end) - Utv(1:end-1,2:end); % y (vert) differences
    minimize(norm([Ux(:); Uy(:)], 1));% + gamma*norm(Ul2(Known)-Uorig(Known),2)); % tv roughness measure
cvx_end

%%%%%

% Graph everything.
figure(1); cla;
%colormap gray;

subplot(221);
imagesc(Uorig)
title('Original image');
axis image;

subplot(222);
imagesc(Known.*Uorig + 256-150*Known);
title('Obscured image');
axis image;

subplot(223);
imagesc(Ul2);
title('l_2 reconstructed image');
axis image;

subplot(224);
imagesc(Utv);
title('Total variation reconstructed image');
axis image;
