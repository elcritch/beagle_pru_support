defmodule Pru.Mixfile do
  use Mix.Project

  @app :beagle_pru_support

  def project do
    [
      name: @app,
      description:
        "Basic library that enables easy interaction with the PRU cores present in the BeagleBone Black.",
      app: @app,
      version: "0.7.0",
      nerves_package: nerves_package(),
      elixir: "~> 1.6",
      compilers: [:nerves_package] ++ Mix.compilers(),
      make_clean: ["clean"],
      start_permanent: Mix.env() == :prod,
      package: hex_package(),
      aliases: [loadconfig: [&bootstrap/1]],
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  def aliases("host"), do: []

  defp hex_package do
    [
      maintainers: ["Jaremy Creechley"],
      files: package_files(),
      licenses: ["Apache-2.0"],
      links: %{"Github" => "https://github.com/elcritch/#{@app}"}
    ]
  end

  def nerves_package do
    [
      name: @app,
      type: :extras_toolchain,
      platform: NervesExtras.Toolchain,
      toolchain_extras: [
        env_var: "PRU_LIB",
        build_link_path: "",
        clean_files: ["priv"],
        archive_script: "scripts/archive.sh"
      ],
      platform_config: [],
      target_tuple: :arm_unknown_linux_gnueabihf,
      artifact_sites: [
        {:github_releases, "elcritch/#{@app}"}
      ],
      checksum: package_files()
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:ex_doc, ">= 0.0.0", only: :dev},
      {:toolchain_extras_pru_cgt, "~> 3.0.1"},
    ]
  end

  defp package_files() do
    [
      "Makefile",
      "LICENSE",
      "mix.exs",
      "README.md",
      "scripts",
      "pru/include/ReadMe.txt"
    ] ++
      Path.wildcard("pru/**/*.c") ++
      Path.wildcard("pru/**/*.h") ++
      Path.wildcard("pru/**/*.cpp") ++
      Path.wildcard("pru/**/*.hpp") ++ Path.wildcard("pru/**/Makefile")
  end

  def aliases() do
    [] |> Nerves.Bootstrap.add_aliases()
  end

  defp bootstrap(args) do
    System.put_env("MIX_TARGET", "CC")
    Application.start(:nerves_bootstrap)
    Mix.Task.run("loadconfig", args)
  end
end
