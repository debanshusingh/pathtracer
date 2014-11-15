#version 150
// ^ Change this to version 130 if you have compatibility issues

//these are the interpolated values out of the rasterizer, so you can't know
//their specific values without knowing the vertices that contributed to them
in vec4 fs_Normal;
in vec4 fs_LightVector;
in vec4 fs_Color;
in vec4 fs_LightColor;
in vec4 fs_ViewVector;

out vec4 out_Color;


void main()
{
    // Material base color (before shading)
    vec4 diffuseColor = fs_Color;
    vec4 specularColor = vec4(1,1,1,1);

    float Ks = 0.8;
    float Kd = 0.8;
    float roughness = .2;
    float gaussConstant = 100;

    vec4 Nn = normalize(fs_Normal);
    vec4 Ln = normalize(fs_LightVector);
    vec4 Vn = normalize(fs_ViewVector);
    vec4 Hn = normalize(Ln+Vn);

    float NdotV = dot(Nn,Vn);
    float NdotL = max(dot(Nn,Ln), 0.0);
    float NdotH = dot(Nn,Hn);
    float VdotH = dot(Vn,Hn);
    
    float alpha = acos(NdotH);
    
    float fresnel = 0.8 + 0.2*pow(1.0 - VdotH, 5.0);    // fresnel - schlick's approximation
    
    //    float roughnessDist = gaussConstant*exp(-(alpha*alpha)/(roughness*roughness));    // roughness - gaussian distribution

    float r1 = 1.0 / ( 4.0 * roughness*roughness * pow(NdotH, 4.0)); // roughness - beckmann distribution
    float r2 = (NdotH*NdotH - 1.0) / (roughness*roughness * NdotH*NdotH);
    float roughnessDist = r1 * exp(r2);
    
    float geomAtt = min(1, min((2*NdotH*NdotV/VdotH),(2*NdotH*NdotL/VdotH)));     // geomteric attenuation
    
    float specularTerm = (fresnel*roughnessDist*geomAtt)/(NdotV*NdotL);
    specularTerm = clamp(specularTerm, 0, 1);
    
    float diffuseTerm = clamp(NdotL, 0, 1);
    
    float ambientTerm = 0.2f;
    
    out_Color = (ambientTerm*diffuseColor + Kd*diffuseColor*diffuseTerm + Ks*specularColor*specularTerm); // both diffuse and specular
}
