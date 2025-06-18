function [norm] = normalization(org, min, max)
    %数据归一化处理，归一化到[-1,1]
    norm = (((org - min) / (max - min)) - 0.5) * 2;
end