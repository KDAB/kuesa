#!/bin/bash -eux

GLSLANG=$(command -v glslangValidator)
SPIRVCROSS=$(command -v spirv-cross)

if [[ "x$GLSLANG" == "x" ]]; then
  return 1
fi

if [[ "x$SPIRVCROSS" == "x" ]]; then
  return 1
fi

SHADERS="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"

SHADER_NAMES=(dof)

for shader in "${SHADER_NAMES[@]}"; do
  # Convert the GL 4.5 shader into SPIR-V
  $GLSLANG -V $shader.frag --target-env opengl -o $shader.spirv
  
  # Convert the SPIR-V representation into the various GL shaders
  $SPIRVCROSS --version '330 core' --no-420pack-extension --no-es $shader.spirv > "$SHADERS/gl3/$shader.frag"
  $SPIRVCROSS --es --version 300 dof.spirv > "$SHADERS/es3/$shader.frag"
  $SPIRVCROSS --es --version 100 dof.spirv > "$SHADERS/es2/$shader.frag"
  
  # For some reason this variable gets an underscore prepended to it...
  sed -i 's/_textureSize/textureSize/g' "$SHADERS/gl3/$shader.frag"
  sed -i 's/_textureSize/textureSize/g' "$SHADERS/es2/$shader.frag"
  sed -i 's/_textureSize/textureSize/g' "$SHADERS/es3/$shader.frag"
  
  rm $shader.spirv
done
