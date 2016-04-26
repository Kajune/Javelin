struct PS_INPUT {
	float4 Pos	: SV_POSITION;
	float4 Col	: COLOR;
};

PS_INPUT VS( float4 pos : POSITION, float4 col : COLOR ) {
	PS_INPUT output;
	output.Pos = pos;
	output.Col = col;
	return output;
}

float4 PS( PS_INPUT input ) : SV_TARGET {
	return input.Col;
}
