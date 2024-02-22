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
data = load("lig_txt/NewData3.txt");

lat = data(:,2);
lon = data(:,3);
sq = data(:, 5);

lat = lat(sq<7);
lon = lon(sq<7);

figure;
worldmap([10 40],[100 135]);  % 修改地图范围
load coastlines;

plotm(coastlat, coastlon, 'k')

hold on;

    scatterm(lat, lon, 3, 'filled','b');
  
    % legend('b','optimize');

data = load("lig_txt/NewData2.txt");
lat = data(:,2);
lon = data(:,3);
sq = data(:, 5); 

lat = lat(sq<7);
lon = lon(sq<7);

% plotm(coastlat, coastlon, 'k')

hold on;

    scatterm(lat, lon, 3, 'filled','r');



%%
% 读取json格式的文本文件
fid = fopen('lig_txt/USTCData.txt');
raw = fread(fid,inf);
str = char(raw');
fclose(fid);
data = jsondecode(str);

lat = [];
lon = [];
for i = 1:size(data)
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
title('添加优化边界限制')




