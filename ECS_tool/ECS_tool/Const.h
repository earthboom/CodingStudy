#pragma once

const WORD MAX_NAME = 256;

// Constant buffers must be 256-byte aligned.
static const UINT c_alignedConstantBufferSize = (sizeof(ECS_tool::ModelViewProjectionConstantBuffer) + 255) & ~255;
