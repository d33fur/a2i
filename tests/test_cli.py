import subprocess
import unittest
import os
import shutil

class TestCLI(unittest.TestCase):
    
    def setUp(self):
        self.executable = "a2i"
    
    def run_command(self, args):
        result = subprocess.run([self.executable] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        return result
    
    def test_help(self):
        result = self.run_command(["--help"])
        self.assertEqual(result.returncode, 0)
        self.assertIn("Usage:", result.stdout)
    
    def test_version(self):
        result = self.run_command(["--version"])
        self.assertEqual(result.returncode, 0)
        self.assertIn("a2i version", result.stdout)
    
    def test_config_reset_default(self):
        result = self.run_command(["config", "reset-default"])
        self.assertEqual(result.returncode, 0)
        self.assertIn("Default configuration successfully saved in ~/.a2i/config.json", result.stdout)
    
    def test_invalid_command(self):
        result = self.run_command(["invalid_command"])
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Unknown command", result.stderr)

    def test_missing_config_file(self):
        config_path = os.path.expanduser("~/.a2i/config.json")
        backup_path = config_path + ".bak"
        if os.path.exists(config_path):
            shutil.move(config_path, backup_path)

        try:
            result = self.run_command(["config", "reset-default"])
            self.assertEqual(result.returncode, 0)
            self.assertIn("Failed to open config file", result.stderr)
        finally:
            if os.path.exists(backup_path):
                shutil.move(backup_path, config_path)

if __name__ == '__main__':
    unittest.main()
