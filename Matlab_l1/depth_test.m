clear; clc;

load('depth_raw.mat');
Uorig = depth_raw;

[m, n] = size(Uorig);

% Create mask of known pixels.
rand('state', 1029);
Known_1 = rand(m,n) > 0.9;
Known_2 = Uorig>100;
Known = Known_1 | Known_2;

figure(1); cla;
imagesc(Uorig)
title('Original image');