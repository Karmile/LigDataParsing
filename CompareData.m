close all
clear all
% figure;
% worldmap([10 40],[100 135]);  % 修改地图范围
% load coastlines;
% 
% plotm(coastlat, coastlon, 'k')
% 
% hold on;
% lat =[0.514705122,0.569870949,0.523494065]*57.3248;
% lon = [2.07061672,2.01754308,1.88706219]*57.3248;
%     scatterm(lat, lon, 3, 'filled','r');
%% NEwData
data = load("lig_txt/NewData.txt");

lat = data(:,2);
lon = data(:,3);
sq = data(:, 5);

lat_g = lat(sq<7);
lon = lon(sq<7);

figure;
worldmap([10 40],[100 135]);  % 修改地图范围
load coastlines;

plotm(coastlat, coastlon, 'k')

hold on;

    scatterm(lat_g, lon, 3, 'filled','b');
  
    % legend('b','optimize');

data = load("lig_txt/NewData2.txt");
lat_o = data(:,2);
lon = data(:,3);
sq = data(:, 5); 

% lat = lat(sq<5);
% lon = lon(sq<5);

% plotm(coastlat, coastlon, 'k')

hold on;

    scatterm(lat_o, lon, 3, 'filled','r');



%%
% 读取json格式的文本文件
fid = fopen('lig_txt/USTCData.txt');
raw = fread(fid,inf);
str = char(raw');
fclose(fid);
data = jsondecode(str);

lat = [];
lon = [];
u_size = size(data);
for i = 1:u_size
    lat = [lat,str2double(data{i}{2})];
    lon = [lon,str2double(data{i}{3})];
end
% figure;
% worldmap([10 40],[100 135]);  % 修改地图范围
% load coastlines;
% 
% plotm(coastlat, coastlon, 'k')

hold on;

scatterm(lat, lon, 3, 'filled', 'g')
title('定位结果对比')





