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
data = load("lig_txt/NewData2_T6_240317_RT.0.3_QZL.txt");
lat = data(:,2);
lon = data(:,3);
sq = data(:, 5);
idx = lat>10&lat<40&lon>100&lon<135;
lat = lat(idx);
lon = lon(idx);
sq1 = sq(idx);
% scatter(lon, lat, 3, sq1, 'filled','b');

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
uri = matlab.net.URI('http://222.195.83.28:18000/getHistoricalDataApi/20240304T160000/20240305T160000/str');
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

% Set histogram properties
legend('T5_GPU+OP2', 'USTC');
title('定位结果对比');
xlabel('定位误差');
ylabel('定位点数');