{ lib, stdenv, pkg-config, gtk4, gtk4-layer-shell }:

stdenv.mkDerivation {
  pname = "hitori-bar";
  version = "0.1.0";
  src = ./.;

  nativeBuildInputs = [ pkg-config ];
  buildInputs = [ gtk4 gtk4-layer-shell ];

  buildPhase = ''
    make CC=${stdenv.cc.targetPrefix}cc
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp hitori-bar $out/bin/
  '';

  meta = with lib; {
    description = "A minimal Wayland status bar";
    longDescription = ''
      A minimal GTK4 Layer Shell status bar for Wayland compositors,
      featuring clock, battery, and volume indicators.
    '';
    homepage = "https://github.com/jihoo12/hitori-bar";
    license = licenses.asl20;
    platforms = platforms.linux;
    maintainers = [ ];
  };
}
