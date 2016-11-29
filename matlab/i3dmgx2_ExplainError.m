function i3dmgx2_ExplainError(Error)
%Display an appropriate error message when an error occurs
%
%Arguments: Error - Error number

ErrorText = i3dmgx2_Errors;
fprintf(['\nError: ' char(ErrorText(Error)) '\n']);
end