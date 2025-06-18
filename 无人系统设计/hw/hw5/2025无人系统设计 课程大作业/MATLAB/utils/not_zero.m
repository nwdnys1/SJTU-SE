function [result] = not_zero(num)
    % 判断0
    if(abs(num) < 0.0000001)
        num = 0.0000001;
    end
    result = num;
end