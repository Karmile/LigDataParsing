close all
clear all

% Load map
MAP = shaperead('.\lig_txt\boundarys\bou2_4p.shp');
figure;

% Plot map
for i = 1:length(MAP)
    plot(MAP(i).X,MAP(i).Y,'k');
    hold on;
end

% Load and filter data
data = load("lig_txt/NewData2_TestHeight.txt");
lat = data(:,2);
lon = data(:,3);
h = data(:,4);
sq = data(:, 5);
idx = lat>10&lat<40&lon>100&lon<135;
lat = lat(idx);
lon = lon(idx);
sq1 = sq(idx);
h = h(idx);
scatter(lon, lat, 3, sq1, 'filled','b');

% Load and filter second data set
data = load("lig_txt/NewData2.txt");
lat = data(:,2);
lon = data(:,3);
h = data(:,4);
sq = data(:, 5);
idx = lat>10&lat<40&lon>100&lon<135;
lat = lat(idx);
lon = lon(idx);
h = h(idx);
sq2 = sq(idx);

% Plot second data set
% scatter(lon, lat, 3, sq2, 'filled','r');
scatter(h, sq2, 3, sq2, 'filled','r');
