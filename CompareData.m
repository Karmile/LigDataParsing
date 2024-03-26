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

% % Load and filter data
data = load("lig_txt/031702_standard.txt");
lat = data(:,2);
lon = data(:,3);
sq = data(:, 5);
idx = lat>10&lat<40&lon>100&lon<135;
lat = lat(idx);
lon = lon(idx);
sq1 = sq(idx);
scatter(lon, lat, 3, sq1, 'filled','y');

% Load and filter second data set
data = load("lig_txt/NewData2.txt");
lat = data(:,2);
lon = data(:,3);
sq = data(:, 5);
idx = lat>10&lat<40&lon>100&lon<135;
lat = lat(idx);
lon = lon(idx);
sq2 = sq(idx);

% Plot second data set
scatter(lon, lat, 3, sq2, 'filled','r');

% Send request and decode response
request = matlab.net.http.RequestMessage;
% uri = matlab.net.URI('http://222.195.83.28:18000/getHistoricalDataApi/20240304T160000/20240305T160000/str');
uri = matlab.net.URI('http://222.195.83.28:18000/getHistoricalDataApi/20240305T040000/20240305T043000/str');

% uri = matlab.net.URI('http://112.26.166.27:18000/getHistoricalLocationData/3/20240323093300/20240323145600?tag=240317_RT.0.4');

response = sendRequest(uri,request);
data = jsondecode(response.Body.Data);

% Extract and plot data from response
lat = [];
lon = [];
sqo = [];
for i = 1:size(data)
    lat = [lat,str2double(data{i}{2})];
    lon = [lon,str2double(data{i}{3})];
    sqo = [sqo,str2double(data{i}{5})];
end
idx = lat>10&lat<40&lon>100&lon<135;
lat = lat(idx);
lon = lon(idx);
sqo = sqo(idx);

scatter(lon, lat, 3, 'filled', 'g')


% Send request and decode response
request = matlab.net.http.RequestMessage;
uri = matlab.net.URI('http://112.26.166.27:18000/getHistoricalLocationData/3/20240323090000/20240323093000?tag=240317_RT.0.3');

response = sendRequest(uri,request);
data = response.Body.Data;

% Extract and plot data from response
lat = [];
lon = [];
sqoJX = [];
for i = 1:size(data)
    lat = [lat,str2double(data{i}{2})];
    lon = [lon,str2double(data{i}{3})];
    sqoJX = [sqoJX,str2double(data{i}{5})];
end
idx = lat>10&lat<40&lon>100&lon<135;
lat = lat(idx);
lon = lon(idx);
sqoJX = sqoJX(idx);

%scatter(lon, lat, 3, 'filled', 'k')

% Set plot properties
title('定位结果对比');
axis equal;
xlim([100 135]);
ylim([10 40])

%% Plot histograms
figure;
hold on;
histogram(sq1, 0:0.025:2.5, 'FaceColor', 'b');
histogram(sq2, 0:0.025:2, 'FaceColor', 'r');
histogram(sqo/6, 0:0.025:2, 'FaceColor', 'g');
%histogram(sqoJX, 0:0.025:2, 'FaceColor', 'k');

% Set histogram properties
legend('T5_GPU+OP2', 'USTC');
title('定位结果对比');
xlabel('定位误差');
ylabel('定位点数');