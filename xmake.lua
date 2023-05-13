---@diagnostic disable: undefined-global, undefined-field
set_project 'cpp_lexer_generator'
set_toolchains 'clang'

add_rules 'plugin.compile_commands.autoupdate'
add_rules('mode.debug', 'mode.release')
add_requires(
    'fmt',
    'gtest',
    'benchmark', -- TODO : add benchmark
    'range-v3'   -- TODO :
)
add_includedirs 'include'
set_languages 'cxxlatest'
set_targetdir '$(projectdir)/bin'

-- INFO :
--  ╭──────────────────────────────────────────────────────────╮
--  │                         pre_hook                         │
--  ╰──────────────────────────────────────────────────────────╯
local old_target = target
local all_targets = {}
target = function(name)
    all_targets[#all_targets + 1] = name
    old_target(name)
end


add_packages('fmt', 'range-v3')
-- Debug模式设置
if is_mode 'debug' then
    set_optimize 'none'
    set_symbols 'debug'
end

-- Release模式设置
if is_mode 'release' then
    set_optimize 'fastest'
    set_symbols 'hidden'
    set_strip 'all'
end

--  ╭──────────────────────────────────────────────────────────╮
--  │                      target define                       │
--  ╰──────────────────────────────────────────────────────────╯
---@format disable
target("lexer_generator")
    set_kind("binary")
    add_files("src/*.cpp")


local test_cases = {
    util = {
    },
    nfa = {
    },
}
for name, option in pairs(test_cases) do
    local target_name = 'test_' .. name
    target(target_name)
        set_kind('binary')
        set_group('test')
        add_files('test/' .. target_name .. '.cpp')
        add_packages('gtest')

    for method, opt in pairs(option) do
        _G[method](opt)
    end
end

task('debug')
    on_run(function()
        import('core.tool.compiler')
        import("core.base.option")

        for _, target_name in ipairs(all_targets) do
            if option.get(target_name) then
                -- build target
                os.exec('xmake build ' .. target_name)


                local projectdir = os.projectdir()
                local debugdir = projectdir .. '/debug'
                local cmd = format('gdb %s/bin/%s', projectdir, target_name)

                -- check and run init script
                -- local init_script = debugdir .. '/' .. target_name .. '_init'
                -- if os.isfile(init_script) then
                --     os.run(format("zsh -c '%s &'", init_script))
                -- end

                -- check and run debug script
                local debug_script = debugdir .. '/' .. target_name .. '.gdb'
                if os.isfile(debug_script) then
                    cmd = cmd .. ' -x ' .. debug_script
                end
                os.exec(cmd)

                -- kill init script
                -- os.exec('pkill -f ' .. init_script)
                break
            end
        end
    end)

    local menu_options = {}
    for i, target_name in ipairs(all_targets) do
        -- local lower_name = name:lower()
        menu_options[i] = {nil, target_name, "k", nil, "Debug " .. target_name}
        -- menu_options[i] = {lower_name:sub(1, 1), lower_name, "k", nil, "Debug " .. name}
    end

    set_menu {
        usage = "xmake debug [options] [target]",
        description = "Debug target",
        options = menu_options
    }

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro defination
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
