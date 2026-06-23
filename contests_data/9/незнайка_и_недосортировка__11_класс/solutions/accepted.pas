program NEDOSORT11(input, output);
(*uses crt;*)
type    number = array [1..50] of byte;
var  N, I, NUM1, NUM2 : word;
     FLAG : boolean;
     ACURR, APREV : number;
procedure readNumber(var A : number);
var CH : char; I, J : word;
begin for I := 1 to 50 do A[I] := 0;
    read(CH);
    I := 0;
    J := 0;
    while (I < 50) do begin
      case CH of
        'A'..'Z' : begin J := J + 1; A[J] := ord(CH) - ord('A') end;
        'a'..'z' : begin J := J + 1; A[J] := ord(CH) - ord('a') + 26 end;
        '^' : A[J] := A[J] + 52;
        '~' : A[J] := A[J] + 104;
        '_' : A[J] := A[J] + 156;
        end;
        if not (eoln or eof) then begin
            read(CH);
            I := I + 1
        end else I := 50
    end;
    for I := J downto 1 do A[50 - J + I] := A[I];
    for I := 1 to 50 - J do A[I] := 0;
    for I := 50 downto 2 do begin
        A[I - 1] := A[I - 1] + A[I] div 52;
        A[I] := A[I] mod 52
    end;
      (*writeln;
	for I := 1 to 50 do write(A[I], ' ');
	writeln;*)
end;

begin
    readln(N);
    NUM1 := 0;
    NUM2 := 0;
    FLAG := FALSE;
    readNumber(ACURR);
    readln;
    I := 1;
    repeat
        APREV := ACURR;
        readNumber(ACURR);
        (* writeln(CompareByte(APREV, ACURR, 50)); *)
        if (CompareByte(APREV, ACURR, 50) < 0) then begin
             if (NUM1 = 0) then NUM1 := I
             else begin
                 NUM2 := I;
                 FLAG := TRUE
             end;
        end;
        I := I + 1;
        if (I < N) then readln
    until (I = N) or FLAG;
    if FLAG then write(NUM1, ' ', NUM2 + 1)
    else write(NUM1, ' ', NUM1 + 1)
end.
