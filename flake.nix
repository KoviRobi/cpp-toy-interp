{
  description = "A toy interpreter to practice my C++";

  outputs = { self, nixpkgs }:
    let

      # to work with older version of flakes
      lastModifiedDate = self.lastModifiedDate or self.lastModified or "19700101";

      # Generate a user-friendly version number.
      version = builtins.substring 0 8 lastModifiedDate;

      # System types to support.
      supportedSystems = [ "x86_64-linux" "x86_64-darwin" "aarch64-linux" "aarch64-darwin" ];

      # Helper function to generate an attrset '{ x86_64-linux = f "x86_64-linux"; ... }'.
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

      # Nixpkgs instantiated for supported system types.
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; overlays = [ self.overlay ]; });

    in

    {

      # A Nixpkgs overlay.
      overlay = final: prev: {

        tiny-interp = with final; stdenv.mkDerivation rec {
          pname = "tiny-interp";
          inherit version;

          src = ./.;

          nativeBuildInputs = [
            cmake
            ninja
            readline
            pkg-config
            catch2_3
          ];
          passthru.devTools = [
            ccls
            cmake-language-server
            clang-tools
            valgrind
          ];
        };

      };

      # Access the nixpkgs with overlays if necessary
      legacyPackages = forAllSystems (system:
        {
          nixpkgs = nixpkgsFor.${system};
        });

      # Provide some binary packages for selected system types.
      packages = forAllSystems (system:
        {
          inherit (nixpkgsFor.${system}) tiny-interp;
        });

      # The default package for 'nix build'. This makes sense if the
      # flake provides only one package or there is a clear "main"
      # package.
      defaultPackage = forAllSystems (system: self.packages.${system}.tiny-interp);

      # Development environment
      devShell = forAllSystems (system:
        let inherit (self.packages.${system}) tiny-interp;
          nixpkgs = nixpkgsFor.${system};
        in
        nixpkgs.mkShell {
          packages = tiny-interp.passthru.devTools;
          inputsFrom = [ tiny-interp ];
          # To allow debugging without optimizations
          hardeningDisable = [ "fortify" ];
        });
    };
}
