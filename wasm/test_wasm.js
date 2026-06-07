const TetWeaveModule = require('../docs/tetweave_wasm.js');

TetWeaveModule().then(module => {
    const generate = module.cwrap('generate_mesh_str', 'string', ['string', 'number']);
    const result = generate('--mesh-type rod --length 5', 0);
    console.log(result.substring(0, 300));
});